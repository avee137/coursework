#!/usr/bin/env python
import itertools
import copy
import sys

input_file = "t_rosalind_long.txt"
input_file = "rosalind_long.txt"

def parse_fasta():
	with open(input_file, 'r') as f:
		lines=f.read()
		lines = lines.split('>')
		data_list = []
		for line in lines:
			if(line):
				line = line.split()
				data = "".join(line[1:])
				data_list.append(data)
	return data_list

def new_ps_match(x,y):
        l = len(y);
	#print "entered  match"
	count = 0
	#print l
        while (l > 0):
		#print "looping in match"
                if (x.endswith(y[:l])):
                        break
                else:
                        #pdb.set_trace()
                        l = l-1
		count += 1
		if count > 6250000:
			sys.exit()
			pdb.set_trace()
        return l  

def new_get_super_dna():
	data_list = parse_fasta()
	#data_list = ["ATGCAGGTGG","GGGCATTTC","TTTAAAGGGTTTC"]
	op_list = copy.deepcopy(data_list)
	count = 0 
	#print op_list
	while (len(op_list) > 1):
		#print "looping in get_super_dna"	
		max_matched_length = 0
		string1 = "" # merged would be string1string2 with suffix and prefix adjustment
		string2 = ""
		for x,y in itertools.permutations(op_list,2):
			l = new_ps_match(x,y)
			if l > max_matched_length :
				max_matched_length = l
				string1 = x
				string2 = y
			
		#if no overlap found after all iterations, work on last unmatched pair
		if (max_matched_length == 0):
			max_matched_length = l
			string1 = x
			string2 = y
			
		#print max_matched_length,"\n", string1,"\n", string2	
		merged_string = string1+string2[max_matched_length:]
		if string1 in op_list:# checks to handle empty case
			op_list.remove(string1)
		if string2 in op_list:
			op_list.remove(string2)
		if merged_string:
			op_list.append(merged_string)
		#print op_list[0]
		#return op_list
		count += 1
		if count >6250000:
			pdb.set_trace()
	return op_list

if __name__=="__main__":
	super_dna = new_get_super_dna()
	print super_dna[0]
	#data_list = new_parse_fasta()
