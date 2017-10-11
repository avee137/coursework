#!/usr/bin/env python
import math

input_file = 'rosalind_prob.txt'
#input_file = 't_rosalind_prob.txt'

def get_prob():
	with open(input_file, 'r') as f:
		ip = f.readlines()
		num_at =  ip[0].count('A') + ip[0].count('T')
		num_cg =  ip[0].count('C') + ip[0].count('G')
		prob_list = []
		for gc_content in ip[1].split():
			p = (((1-float(gc_content))/2) **(num_at)) * ((float(gc_content)/2) ** (num_cg))
			prob_list.append(p)
		log_prob_list = []
		for p in prob_list:
			log_prob_list.append(math.log10(p))
		return log_prob_list	

if __name__=="__main__":
	log_prob_list = get_prob()
	for pl in log_prob_list:
		print "%.3f" %pl,	
