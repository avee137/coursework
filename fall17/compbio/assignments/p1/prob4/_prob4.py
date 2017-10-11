#!/usr/bin/env python
import itertools
import copy
import sys

input_file = "t_rosalind_long.txt"
#input_file = "rosalind_long.txt"

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

#prefix-suffix match
def ps_match(x, y):
	merged_str = ""
	l = 1 #start with first char
	matched_l = 0
	while (l < len(y)):
		if x.endswith(y[:l]):
			#print "suffix x, prefix y"
			l += 1
		else:
			break

	matched_l = l-1
	l=1
	while (l < len(x)):
		if y.endswith(x[:l]):
			#print "suffix y, prefix x"
			l += 1
		else:
			break
		
	if l-1 > matched_l : # suffix from y matches prefix from x
		matched_l = l-1
		merged_str = y+x[matched_l:] 
	else:		# suffix from x matches prefix from y
		merged_str = x+y[matched_l:]
	#print "matched max %d" %max_l 
	return matched_l, merged_str
		

# greedy approximation
def get_super_dna():
	data_list = parse_fasta()
	tmp_data_list = copy.deepcopy(data_list)
	while (len(tmp_data_list) > 1):
		#print len(tmp_data_list)
		data_pairs = itertools.combinations(tmp_data_list,2)
		#print len(list(data_pairs))
		#sys.exit(0)
		max_match_len = 0
		s1=""
		s2=""
		merged=""
		#print "data list len %d" %(len(tmp_data_list))
		for x,y in data_pairs:
			ps_match_len,new_string = ps_match(x,y)
			#print "matched len %d, max matchedlen %d" %(ps_match_len,max_match_len) 
			#print new_string
			if ps_match_len > max_match_len :
				#print "replacing"	
				max_match_len = ps_match_len
				s1 = x
				s2 = y
				merged = new_string
			if (ps_match_len == 0) and (len(tmp_data_list) == 2):
				s1 = x
				s2 = y
				merged = new_string
				print s1, s2, merged	
			#print "looping in for"

		if s1 and s2 and merged:
			tmp_data_list.remove(s1)
			tmp_data_list.remove(s2)
			tmp_data_list.append(merged)
			#print s1, s2, merged, len(tmp_data_list)	
		#print "looping in while"
	return tmp_data_list[0]

def new_ps_match(x,y):
        l = len(y);
        while (l > 0):
                if (x.endswith(y[:l])):
                        break
                else:
                        #pdb.set_trace()
                        l -= 1
        return l  

def new_get_super_dna():
	data_list = parse_fasta()
	#data_list = ["ATGCAGGTGG","GGGCATTTC","TTTAAAGGGTTTC"]
	op_list = copy.deepcopy(data_list)
	count = 0 
	#print op_list
	while (len(op_list) > 1):	
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
