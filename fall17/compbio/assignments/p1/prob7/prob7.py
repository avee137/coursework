#!/usr/bin/env python
import itertools

input_file = 't_rosalind_grph.txt'
#input_file = 'rosalind_grph.txt'

data_list = []
label_list = []
def parse_fasta():
        with open(input_file, 'r') as f:
                lines = f.readlines()
                data = ""
                for line in lines:
                        if line[0] == '>':
                                if data is not "":
                                        data_list.append(data)
                                        data = ""
				label_list.append(line.lstrip('>').rstrip())
                        else:
                                data += line.rstrip()
                #print data_list
                data_list.append(lines[-2].rstrip()+lines[-1].rstrip()) # last two lines would not be followed by another label                        
	nodes = {}
        for k,v in zip(label_list,data_list) :
		nodes[k] = v
	return  nodes

def new_get_nodes():	
	nodes = parse_fasta()
	#print nodes
	graph_nodes = []
	for k1,v1 in nodes.items():
		for k2,v2 in nodes.items():
			if (k1 != k2) and (v1[-3:] == v2[:3]) :
				print k1,k2 

def get_nodes():
        nodes = parse_fasta()
        #for k,v in nodes.items():
        #       print k,v
        node_pairs = itertools.combinations(sorted(nodes),2)
        #for k,v in node_pairs:
                #print k,v
        #print type(node_pairs)
        graph_node_pairs = []
        for x,y in node_pairs:
                #print x,type(x),y,type(y)
                #print x,nodes[x],nodes[x][0:3]
                #print y,nodes[y],nodes[y][-3:]
                if (nodes[x][-3:] == nodes[y][0:3]): 
                        graph_node_pairs.append((x,y))
		if (nodes[y][-3:] == nodes[x][0:3]):
                        graph_node_pairs.append((y,x))
        #print "\t\t %s  ::  %s"%(nodes[l[0]],(nodes[l[1]]))
        return graph_node_pairs
	
if __name__=="__main__":
	#print "main" 
	graph_node_pairs = get_nodes()
        for each in graph_node_pairs:
                l = list(each)
                print l[0],l[1]

	
