/* Submitter : Avinash Kumar
 * sbu ID : 111471353
 * net ID : avkumar
 */
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
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <time.h>

#define SIZE_ETHERNET 14
#define IP_HL(ip) (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip) (((ip)->ip_vhl) >> 4)
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPV4 0x0800

typedef enum { false, true } boolean;

/* Ethernet header */
struct sniff_ethernet
{
	u_char  ether_dhost[ETHER_ADDR_LEN];	/* destination host address */
	u_char  ether_shost[ETHER_ADDR_LEN];	/* source host address */
	u_short ether_type;		        /* IP? ARP? RARP? etc */
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

//input search string
static char *srch_str;

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

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	const struct sniff_ethernet *ethernet;
	const struct sniff_ip *ip;
	const struct sniff_tcp *tcp;
	const struct sniff_udp *udp;
	const char *payload;
	
	int size_ip=0;
	int size_tcp=0;
	int size_udp = 8; // udp header length
	int size_icmp = 8; //icmp header length
	int size_payload=0;
	
        /*print ISO 8601 TS*/
	/*print micor seconds*/
	time_t raw_time = (time_t)header->ts.tv_sec;
	time(&raw_time);
	char buf[64]= {'\0'}, tmbuf[96]={'\0'};
	strftime(buf, sizeof(buf), "%F %T",gmtime(&raw_time)); 
        sprintf(tmbuf,"%s.%06ld",buf,header->ts.tv_usec);
        	
        /*print ethernet src,dst,type, len*/
        /*00:1E:4F:A6:2D:77 -> 00:00:5E:00:01:64 type 0x800 len 98*/	
	char ether_header[256]={'\0'};
	ethernet = (struct sniff_ethernet*)(packet);
        sprintf(ether_header,"%s %s -> %s type 0x%04X len %d\n",
		tmbuf, ether_ntoa((struct ether_addr *)ethernet->ether_shost),
		ether_ntoa((struct ether_addr *)ethernet->ether_dhost),
		ethernet->ether_type,header->len); 	
	
	/*print 10.0.0.1:137 -> 10.0.0.255:137 UDP */
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}
	switch (ip->ip_p) {
		case IPPROTO_IP:

			/*print payload*/
			payload = (u_char *)(packet + SIZE_ETHERNET);
			size_payload = ntohs(ip->ip_len);
			if (size_payload > 0){
				if((NULL == srch_str) || (NULL != strstr(payload, srch_str))){
					printf("%s", ether_header);
					printf("%s -> %s %s\n", inet_ntoa(ip->ip_src),inet_ntoa(ip->ip_dst),"IP");	
					print_payload(payload, size_payload);
				}
			}	
			break;

		case IPPROTO_TCP:
			tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
			size_tcp = TH_OFF(tcp)*4;
			if (size_tcp < 20) {
				printf("* Invalid TCP header length: %u bytes\n", size_tcp);
				return;
			}

			/*print payload*/
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
			size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
			if (size_payload > 0){
				if((NULL == srch_str) || (NULL != strstr(payload, srch_str))){
					printf("%s", ether_header);
					printf("%s:%d -> %s:%d %s\n", inet_ntoa(ip->ip_src),ntohs(tcp->th_sport),
								      inet_ntoa(ip->ip_dst),ntohs(tcp->th_dport), "TCP");
					print_payload(payload, size_payload);
				}
			}	
			break;

		case IPPROTO_UDP:
			udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);

			/*print payload*/
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_udp);
			size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);
			if (size_payload > 0){
				if((NULL == srch_str) || (NULL != strstr(payload, srch_str))){
					printf("%s", ether_header);
					printf("%s:%d -> %s:%d %s\n", inet_ntoa(ip->ip_src),ntohs(udp->sport),
								      inet_ntoa(ip->ip_dst),ntohs(udp->dport), "UDP");		
					print_payload(payload, size_payload);
				}
			}	
			break;

		case IPPROTO_ICMP:

			/*print payload*/
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_icmp);
			size_payload = ntohs(ip->ip_len) - (size_ip + size_icmp);
			if (size_payload > 0){
				if((NULL == srch_str) || (NULL != strstr(payload, srch_str))){
					printf("%s", ether_header);
					printf("%s -> %s %s\n", inet_ntoa(ip->ip_src),inet_ntoa(ip->ip_dst),"ICMP");		
					print_payload(payload, size_payload);
				}
			}	
			break;

		default:

			/*print payload*/
			payload = (u_char *)(packet + SIZE_ETHERNET + size_ip );
			size_payload = ntohs(ip->ip_len) - (size_ip);
			if (size_payload > 0){
				if((NULL == srch_str) || (NULL != strstr(payload, srch_str))){
					printf("%s", ether_header);
					printf("%s -> %s %s\n", inet_ntoa(ip->ip_src),inet_ntoa(ip->ip_dst),"OTHER");		
					print_payload(payload, size_payload);
				}
			}	
			break;
	}

	return;
}

void print_usage(){
	printf("USAGE: mydump [-i <interface> | -r <input_file>] {-s <search_string>} {<bpf_filter>}\n");
	return;
}

int main(int argc, char *argv[]) {
	int opt = 0;
	char *interface = NULL;
	char *ip_file = NULL;
	char *expression = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;

	struct bpf_program filter;
	char filter_string[] = "ip";
	bpf_u_int32 mask;
	bpf_u_int32 net;
	struct pcap_pkthdr header;
	const u_char *packet;

	while ((opt = getopt(argc, argv, "i:r:s:")) != -1) {
		switch(opt) {
			case 'i':
				interface = optarg;
				break;
			case 'r':
				ip_file = optarg;
				break;
			case 's':
				srch_str = optarg;
				break;
			case '?':
				// no args after option
				if (optopt == 'i') {
					printf("Provide interface!\n");
					return -1;
				} else if (optopt == 'r') {
					printf("Provide file name!\n");
					return -1;
				} else if (optopt == 's') {
					printf("Provide search string!\n");
					return -1;
				} else {
					printf("Unknown arguement!\n");
					return -1;
				}
			default:
				printf("Invalid getopt() case!\n");
				return -1;
		}
		
	}
	
	// get optional exp
	expression = argv[optind];
	//printf("i-%s,r-%s, s-%s, expression -%s\n", interface, ip_file, srch_str, expression);
	
	if (interface != NULL && ip_file != NULL) {
		printf("Provide EITHER interface OR capture file\n");
		print_usage();
		return -1;
	}
	
	if (interface == NULL && ip_file == NULL) {
		print_usage();
		return -1;
	}

	// open interface or file here
	if (NULL != interface) {
		// Start pcap session
		handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
		if (handle == NULL) {
			printf("Error: %s\n", errbuf);
			return -1;
		}
	}	
	if ( NULL != ip_file ) {
		handle = pcap_open_offline(ip_file, errbuf);
		if (handle == NULL) {
			printf("Error: %s\n", errbuf);
			return -1;
		}
	} 
	
	// link layer must be ethernet
	if (pcap_datalink(handle) != DLT_EN10MB) {
		printf("Interface %s is not ethernet.\n", interface);
		return -1;
	}

	
	// compile and apply expression
	if (expression != NULL) {
		// compile filter string
		if (pcap_compile(handle, &filter, expression, 0, net) == -1) {
			printf("Error: %s\n", pcap_geterr(handle));
			return -1;
		}
		// apply filter
		if (pcap_setfilter(handle, &filter) == -1) {
			printf("Error: %s\n", pcap_geterr(handle));
			return -1;
		}
	}
	//capture in loop
	pcap_loop(handle, -1, got_packet, NULL);
	
	//pcap_freecode(&filter);
	pcap_close(handle);
	return 0;
}
