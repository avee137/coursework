#!/usr/bin/env python

input_file = 'rosalind_splc.txt' 
input_file = 't_rosalind_splc.txt' 

def parse_fasta():
	with open(input_file, 'r') as f:
		lines = f.readlines()
		data_list = []
		data = ""
		for line in lines:
			if line[0] == '>':
				print line
				if data is not "":
					print data
					data_list.append(data)
					data = ""
			else:
				#print data
				data += line.rstrip()
		data_list.append(lines[-1].rstrip()) # last line would noe be followed by another label
		#print data_list		
	#print data_list	
	return data_list[0], data_list[1:]

if __name__== "__main__":
	dna, introns = parse_fasta()	
	print dna
	print introns
