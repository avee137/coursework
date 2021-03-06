#include <pcap.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define ETHER_ADDR_LEN 6
#define SIZE_ETHERNET 14
#define IP_HL(ip) (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip) (((ip)->ip_vhl) >> 4)
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPV4 0x0800

typedef enum { false, true } boolean;

// define necessary struct for headers
/* Ethernet header */
struct sniff_ethernet
{
	u_char  ether_dhost[ETHER_ADDR_LEN];	/* destination host address */
	u_char  ether_shost[ETHER_ADDR_LEN];	/* source host address */
	u_short ether_type;					 /* IP? ARP? RARP? etc */
};
/* IP header */
struct sniff_ip
{
	u_char  ip_vhl;				 /* version << 4 | header length >> 2 */
	u_char  ip_tos;				 /* type of service */
	u_short ip_len;				 /* total length */
	u_short ip_id;				  /* identification */
	u_short ip_off;				 /* fragment offset field */
#define IP_RF 0x8000			/* reserved fragment flag */
#define IP_DF 0x4000			/* dont fragment flag */
#define IP_MF 0x2000			/* more fragments flag */
#define IP_OFFMASK 0x1fff	   /* mask for fragmenting bits */
	u_char  ip_ttl;				 /* time to live */
	u_char  ip_p;				   /* protocol */
	u_short ip_sum;				 /* checksum */
	struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
/* UDP header */
struct sniff_udp {
	u_short sport;	/* source port */
	u_short dport;	/* destination port */
	u_short udp_length;
	u_short udp_sum;	/* checksum */
};
/* TCP header */
typedef u_int tcp_seq;
struct sniff_tcp
{
	u_short th_sport;			   /* source port */
	u_short th_dport;			   /* destination port */
	tcp_seq th_seq;				 /* sequence number */
	tcp_seq th_ack;				 /* acknowledgement number */
	u_char  th_offx2;			   /* data offset, rsvd */
#define TH_OFF(th)	  (((th)->th_offx2 & 0xf0) >> 4)
	u_char  th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS		(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
	u_short th_win;				 /* window */
	u_short th_sum;				 /* checksum */
	u_short th_urp;				 /* urgent pointer */
};

void print_hex_ascii_line(const u_char *payload, int len, int offset) {
	int i;
	int gap;
	const u_char *ch;
	
	/* offset */
	printf("%05d   ", offset);
	
	/* hex */
	ch = payload;
	for(i = 0; i < len; i++) {
		printf("%02x ", *ch);
		ch++;
		/* print extra space after 8th byte for visual aid */
		if (i == 7)
			printf(" ");
	}
	/* print space to handle line less than 8 bytes */
	if (len < 8)
		printf(" ");

	/* fill hex gap with spaces if not full line */
	if (len < 16) {
		gap = 16 - len;
		for (i = 0; i < gap; i++) {
			printf("   ");
		}
	}
	printf("   ");

	/* ascii (if printable) */
	ch = payload;
	for(i = 0; i < len; i++) {
		if (isprint(*ch))
			printf("%c", *ch);
		else
			printf(".");
		ch++;
	}

	printf("\n");

	return;
}

void print_payload(const u_char *payload, int len) {
	int len_rem = len;
	int line_width = 16;	/* number of bytes per line */
	int line_len;
	int offset = 0;		/* zero-based offset counter */
	const u_char *ch = payload;

	if (len <= 0)
		return;

	/* data fits on one line */
	if (len <= line_width) {
		print_hex_ascii_line(ch, len, offset);
		return;
	}

	/* data spans multiple lines */
	for ( ;; ) {
		/* compute current line length */
		line_len = line_width % len_rem;

		/* print line */
		print_hex_ascii_line(ch, line_len, offset);

		/* compute total remaining */
		len_rem = len_rem - line_len;

		/* shift pointer to remaining bytes to print */
		ch = ch + line_len;

		/* add offset */
		offset = offset + line_width;

		/* check if we have line width chars or less */
		if (len_rem <= line_width) {
			/* print last line and get out */
			print_hex_ascii_line(ch, len_rem, offset);
			break;
		}
	}

	return;
}

// display info for each packet
void handle_packet(boolean http, char *string, const struct pcap_pkthdr *header,\
	const u_char *packet) {
	/* declare pointers to packet headers */
	const struct sniff_ethernet *ethernet;
	const struct sniff_ip *ip;
	const struct sniff_tcp *tcp;
	const struct sniff_udp *udp;
	const char *payload;
	
	int size_ip;
	int size_tcp;
	int size_udp = 8; // fixed udp header length
	int size_icmp = 8; // fixed icmp header length
	int size_payload;
	
	// print out time stamp. convert elapse time to date,
	// then remove new line symbol at the end
	time_t raw_time = (time_t)header->ts.tv_sec;
	char *ptr = ctime(&raw_time);
	char timebuf[126];
	strcpy(timebuf, ptr);
	timebuf[strlen(timebuf)-1] = 0;
	printf("%s ", timebuf);
	
	// extract ethernet header
	ethernet = (struct sniff_ethernet*)(packet);
	
	if (ntohs(ethernet->ether_type) == ETHERTYPE_IPV4) {
		printf("IPv4 ");
		// extract ip header
		ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
		size_ip = IP_HL(ip)*4;
		if (size_ip < 20) {
			printf("* Invalid IP header length: %u bytes\n", size_ip);
			return;
		}
		
		// for tcp packet
		if (ip->ip_p == IPPROTO_TCP) {
			printf("TCP ");
			tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
			size_tcp = TH_OFF(tcp)*4;
			if (size_tcp < 20) {
				printf("* Invalid TCP header length: %u bytes\n", size_tcp);
				return;
			}
			printf("%s.%d -> ", inet_ntoa(ip->ip_src), ntohs(tcp->th_sport));
			printf("%s.%d ", inet_ntoa(ip->ip_dst), ntohs(tcp->th_dport));
			printf("len %d ", ntohs(ip->ip_len));
			
			// extract payload
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
			size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
			
			// print payload
			if (size_payload > 0) {
				printf("Payload (%d bytes):\n", size_payload);
				
				if (string != NULL) {
					if (strstr(payload, string) == NULL)
						return;
				}
				
				if (http) {
					char tmp[strlen(payload)];
					strcpy(tmp, payload);
					char *ptr = strtok(tmp, " ");
					ptr = strtok(NULL, " ");
					printf("%s\n", ptr);
				} else {
					print_payload(payload, size_payload);
				}
			}
			printf("\n");
		} else if (ip->ip_p == IPPROTO_UDP) {
			printf("UDP ");
			udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
			
			printf("%s.%d -> ", inet_ntoa(ip->ip_src), ntohs(udp->sport));
			printf("%s.%d ", inet_ntoa(ip->ip_dst), ntohs(udp->dport));
			printf("len %d ", ntohs(ip->ip_len));
			
			// extract payload
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_udp);
			size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);
			
			// print payload
			if (size_payload > 0)
			{
				printf("Payload (%d bytes):\n", size_payload);
				print_payload(payload, size_payload);
			}
			printf("\n");
		} else if (ip->ip_p == IPPROTO_ICMP) {
			printf("ICMP ");
			
			printf("%s -> ", inet_ntoa(ip->ip_src));
			printf("%s ", inet_ntoa(ip->ip_dst));
			printf("len %d ", ntohs(ip->ip_len));
			
			// extract payload
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_icmp);
			size_payload = ntohs(ip->ip_len) - (size_ip + size_icmp);
			
			// print payload
			if (size_payload > 0)
			{
				printf("Payload (%d bytes):\n", size_payload);
				print_payload(payload, size_payload);
			}
			printf("\n");
		} else {
			printf("OTHER ");
			// extract payload
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip);
			size_payload = ntohs(ip->ip_len) - (size_ip);
			
			// print payload
			if (size_payload > 0)
			{
				printf("Payload (%d bytes):\n", size_payload);
				print_payload(payload, size_payload);
			}
			printf("\n");
		}
	} else if (ntohs(ethernet->ether_type) == ETHERTYPE_ARP) {
		printf("ARP\n");
	} else {
		printf("OTHER\n");
	}
	
	return;
}

// callback function for pcap_loop
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	// parse args and initialize http and string
	//printf("got packet!\n");
	boolean http = false;
	char *string = NULL;
	if (args != NULL) {
		char g = *args;
		if (g == 'g') {
			http = true;
			if (strlen(args+1) > 0) {
				string = args + 1;
				//printf("There is pattern string!\n");
			}
			//printf("New string: %s\n", string);
		} else {
			string = args;
			//printf("New string: %s\n", string);
		}
	}
	
	if (string == NULL)
		handle_packet(http, string, header, packet);
	else {
		/* declare pointers to packet headers */
		const struct sniff_ethernet *ethernet;
		const struct sniff_ip *ip;
		const struct sniff_tcp *tcp;
		const struct sniff_udp *udp;
		const char *payload;
		
		int size_ip;
		int size_tcp;
		int size_udp = 8; // fixed udp header length
		int size_icmp = 8; // fixed udp header length
		int size_payload;
		
		// extract ethernet header
		ethernet = (struct sniff_ethernet*)(packet);
		
		if (ntohs(ethernet->ether_type) == ETHERTYPE_IPV4) {
			// extract ip header
			ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
			size_ip = IP_HL(ip)*4;
			if (size_ip < 20) {
				return;
			}
			
			// for tcp packet
			if (ip->ip_p == IPPROTO_TCP) {
				tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
				size_tcp = TH_OFF(tcp)*4;
				if (size_tcp < 20) {
					return;
				}
				
				// extract payload
				payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
				size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
				
				// print payload
				if (size_payload > 0) {
					char str_payload[size_payload];
					strncpy(str_payload, payload, size_payload);
					
					if (strstr(str_payload, string) == NULL)
						return;
					else
						handle_packet(http, string, header, packet);
				} else {
					return;
				}
			} else if (ip->ip_p == IPPROTO_UDP) {
				udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
				
				// extract payload
				payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_udp);
				size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);
				
				// print payload
				if (size_payload > 0) {
					char str_payload[size_payload];
					strncpy(str_payload, payload, size_payload);
					
					if (strstr(str_payload, string) == NULL)
						return;
					else
						handle_packet(http, string, header, packet);
				} else {
					return;
				}
			} else if (ip->ip_p == IPPROTO_ICMP) {
				// extract payload
				payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_icmp);
				size_payload = ntohs(ip->ip_len) - (size_ip + size_icmp);
				
				// print payload
				if (size_payload > 0) {
					char str_payload[size_payload];
					strncpy(str_payload, payload, size_payload);
					
					if (strstr(str_payload, string) == NULL)
						return;
					else
						handle_packet(http, string, header, packet);
				} else {
					return;
				}
			} else {
				// extract payload
				payload = (u_char *)(packet + SIZE_ETHERNET + size_ip);
				size_payload = ntohs(ip->ip_len) - (size_ip);
				
				// print payload
				if (size_payload > 0)
				{
					char str_payload[size_payload];
					strncpy(str_payload, payload, size_payload);
					
					if (strstr(str_payload, string) == NULL)
						return;
					else
						handle_packet(http, string, header, packet);
				} else {
					return;
				}
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int opt = 0;
	char *interface = NULL;
	char *ip_file = NULL;
	char *string = NULL;
	boolean http = false;
	char *expression = NULL;
	// this string is used for communication between this
	// program and pcap
	char errbuf[PCAP_ERRBUF_SIZE];
	// this is like file system
	pcap_t *handle;
	// compiled filter
	struct bpf_program filter;
	// http get and post filter before compiling
	char filter_string[] = "(tcp port http) && ((tcp[32:4] = 0x47455420) || \
		(tcp[((tcp[12:1] & 0xf0) >> 2):4] = 0x504f5354))";
	// The netmask of interface
	bpf_u_int32 mask;
	// The ip address of interface
	bpf_u_int32 net;
	// the struct to store packet header
	struct pcap_pkthdr header;
	// the actual packet
	const u_char *packet;
	// set counter to negative so sniffer will continue working
	int cnt = -1;

	while ((opt = getopt(argc, argv, "i:r:s")) != -1) {
		switch(opt) {
			case 'i':
				interface = optarg;
				break;
			case 'r':
				ip_file = optarg;
				break;
			case 's':
				string = optarg;
				break;
			case '?':
				// no args after option
				if (optopt == 'i') {
					printf("Provide interface!\n");
					return 0;
				} else if (optopt == 'r') {
					printf("Provide file name!\n");
					return 0;
				} else if (optopt == 's') {
					printf("Provide search string!\n");
					return 0;
				} else {
					printf("Unknown arguement!\n");
					return 0;
				}
			default:
				printf("Default case?!\n");
				return 0;
		}
		
	}
	
	// get option exp
	if (optind == argc - 1)
		expression = argv[optind];
	else if (optind < argc -1) {
		perror("Redundant arguments. Exiting...\n");
		return 0;
	}
	
	if (interface != NULL && ip_file != NULL) {
		perror("\n");
		return 0;
	}
	
	if (interface == NULL && ip_file == NULL) {
		interface = pcap_lookupdev(errbuf);
		//interface = NULL;
		if (interface == NULL) {
			printf("Error finding default device! Error message: %s\n\
			Exiting...\n", errbuf);
			return 0;
		}
	}

	// open interface or file here
	if (interface != NULL && ip_file == NULL) {
		// Get ip and netmask of sniffing interface or file
		if (pcap_lookupnet(interface, &net, &mask, errbuf) == -1) {
			printf("Error getting ip and mask! Error message: %s\n", errbuf);
			net = 0;
			mask = 0;
		}
		// Start pcap session
		handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
		if (handle == NULL) {
			printf("Error opening live! Error message: %s\n\
			Existing...\n", errbuf);
			return 0;
		}
	} else if (interface == NULL && ip_file != NULL) {
		handle = pcap_open_offline(ip_file, errbuf);
		if (handle == NULL) {
			printf("Error opening file! Error message: %s\n\
			Existing...\n", errbuf);
			return 0;
		}
	} else {
		printf("This shouldn't be printed out! Existing...\n");
		return 0;
	}
	
	// check if link-layer header is ethernet
	if (pcap_datalink(handle) != DLT_EN10MB) {
		printf("Interface %s doesn't using ethernet header! Existing\n", interface);
		return 0;
	}
	
	// set http sniffer mode
	if (http) {
		// compile filter string
		if (pcap_compile(handle, &filter, filter_string, 0, net) == -1) {
			printf("Error compiling http filter! Error message: %s\n\
			Existing...\n", pcap_geterr(handle));
			return 0;
		}
		// apply compiled filter to session
		if (pcap_setfilter(handle, &filter) == -1) {
			printf("Error applying http filer! Error message: %s\n\
			Existing...\n", pcap_geterr(handle));
			return 0;
		}
	}
	
	// compile and apply expression
	if (expression != NULL) {
		// compile filter string
		if (pcap_compile(handle, &filter, expression, 0, net) == -1) {
			printf("Error compiling expression! Error message: %s\n\
			Existing...\n", pcap_geterr(handle));
			return 0;
		}
		// apply compiled filter to session
		if (pcap_setfilter(handle, &filter) == -1) {
			printf("Error applying expression! Error message: %s\n\
			Existing...\n", pcap_geterr(handle));
			return 0;
		}
	}
	
	// now we start sniffing!
	if (http) {
		int len = 0;
		if (string != NULL)
			len = strlen(string);
		char *tmp = (char *)malloc(len+2);
		strcpy(tmp, "g");
		if (string != NULL)
			strcat(tmp, string);
		
		//printf("Old string: %s\n", tmp);
		pcap_loop(handle, cnt, got_packet, tmp);
	} else {
		pcap_loop(handle, cnt, got_packet, string);
	}
	
	//pcap_freecode(&filter);
	pcap_close(handle);
	printf("\n\t... ... the end ... ...\n\n");
	return 0;
}
