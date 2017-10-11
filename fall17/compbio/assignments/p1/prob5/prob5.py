#!/usr/bin/env python
import re

input_file = "rosalind_prot.txt"
#input_file = "t_rosalind_prot.txt"

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
        'UGG': 'W'    ,  'CGG': 'R' ,     'AGG': 'R'   ,   'GGG': 'G',}
	
	with open(input_file, 'r') as f:
		lines = f.readlines()
	
	rna_chain=""
	for line in lines:
		rna_chain += line.rstrip()

	codon_list = re.findall('...', rna_chain)
	protein_string = ""
	for codon in codon_list:
		protein_string += codon_db[codon]
	protein_string = protein_string.replace('Stop', '')
	return protein_string		

if __name__=="__main__":
	pstring = get_protein_string()
	print pstring			
