#!/usr/bin/env python

import re
from Bio import SeqIO

input_file = 'rosalind_splc.txt'
#input_file = 't_rosalind_splc.txt'

def parse_fasta():
        with open(input_file, 'r') as f:
                lines = f.readlines()
                data_list = []
                data = ""
                for line in lines:
                        if line[0] == '>':
                                if data is not "": 
                                        data_list.append(data)
                                        data = ""
                        else:
                                data += line.rstrip()
                #print data_list
		data_list.append(lines[-1].rstrip()) # last line would not be followed by another label                        
        return data_list[0], data_list[1:] # [1,-1] does not include last element 

def get_protein_string():
	codon_db = {'UUU': 'F'   ,   'CUU': 'L',      'AUU': 'I',     'GUU': 'V',
	'UUC': 'F'    ,  'CUC': 'L' ,     'AUC': 'I' ,     'GUC': 'V',
	'UUA': 'L'    ,  'CUA': 'L' ,     'AUA': 'I' ,     'GUA': 'V',
	'UUG': 'L'    ,  'CUG': 'L' ,     'AUG': 'M' ,     'GUG': 'V',
	'UCU': 'S'    ,  'CCU': 'P' ,     'ACU': 'T' ,     'GCU': 'A',
	'UCC': 'S'    ,  'CCC': 'P' ,     'ACC': 'T' ,     'GCC': 'A',
	'UCA': 'S'    ,  'CCA': 'P' ,     'ACA': 'T' ,     'GCA': 'A',
	'UCG': 'S'    ,  'CCG': 'P' ,     'ACG': 'T' ,     'GCG': 'A',
	'UAU': 'Y'    ,  'CAU': 'H' ,     'AAU': 'N' ,     'GAU': 'D',
	'UAC': 'Y'    ,  'CAC': 'H' ,     'AAC': 'N' ,     'GAC': 'D',
	'UAA': 'Stop' ,  'CAA': 'Q' ,     'AAA': 'K' ,     'GAA': 'E',
	'UAG': 'Stop' ,  'CAG': 'Q' ,     'AAG': 'K' ,     'GAG': 'E',
	'UGU': 'C'    ,  'CGU': 'R' ,     'AGU': 'S' ,     'GGU': 'G',
	'UGC': 'C'    ,  'CGC': 'R' ,     'AGC': 'S' ,     'GGC': 'G',
	'UGA': 'Stop' ,  'CGA': 'R' ,     'AGA': 'R' ,     'GGA': 'G',
	'UGG': 'W'    ,  'CGG': 'R' ,   'AGG': 'R'   ,   'GGG': 'G',}

	"""
	# using Bio module
	ip = SeqIO.parse(open(input_file),'fasta')
	ip_data = []
	for each in ip:
		label, data = each.id, str(each.seq)
		ip_data.append(str(each.seq)) # data[0] = dna, data[1 to n] = introns
	dna_string = ip_data[0]
	intron_list = ip_data[1:-1]
	"""
	
	"""	
	#test	
	dna_string = "ATGGTCTACATAGCTGACAAACAGCACGTAGCAATCGGTCGAATCTCGAGAGGCATATGGTCACATGATCGGTCGAGCGTGTTTCAAAGTTTGCGCCTAG"
	intron_list = ["ATCGGTCGAA", "ATCGGTCGAGCGTGT"]
	"""
	dna_string, intron_list = parse_fasta()
	for intron in intron_list:
		dna_string = dna_string.replace(intron, '')
	
	#spliced_dna = (dna_string.replace(intron1, '').replace(intron2, ''))
	spliced_rna = dna_string.replace('T', 'U')
	codon_list = re.findall('...',spliced_rna)
	protein_chain = []
	for each in codon_list:
		protein_chain.append(codon_db[each])

	protein = ''.join(protein_chain).replace('Stop', '')
	return protein

if __name__ == "__main__":
	protein = get_protein_string()
	print protein
