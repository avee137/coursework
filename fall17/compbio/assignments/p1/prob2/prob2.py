#!/usr/bin/env python
from collections import Counter

"""
iterate through the protein string and count the number of possible combinations of
RNA strings which could have resulted into this protein string.
NOTE : this gives the modulo 1000000 of the possible counts. i.e count % 1000000
"""
input_file = 'rosalind_mrna.txt'

def count_rna_strings():
	codon_db =     {'UUU': 'F'   ,   'CUU': 'L',      'AUU': 'I',      'GUU': 'V',
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
			'UGG': 'W'    ,  'CGG': 'R' ,     'AGG': 'R' ,     'GGG': 'G',}

	codon_per_amino = Counter(codon_db.values())
	#print codon_per_amino['M']
	#print codon_per_amino['A']
	#print codon_per_amino['Stop']

	with open (input_file, 'r') as f:
		protein_chain = f.read()
	#test 
	#protein_chain = "MA\n"
	protein_chain = protein_chain.rstrip() #remove white spaces if any
	#print protein_chain
	amino_acid_list = list(protein_chain)
	#print amino_acid_list

	possible_rna_strings = 1
	possible_rna_strings *= codon_per_amino['Stop'] #account for Stop seq
	for each_amino in amino_acid_list:
		possible_rna_strings *= codon_per_amino[each_amino]
		possible_rna_strings %= 1000000
	return possible_rna_strings

if __name__ == "__main__":
	possible_rna_strings = count_rna_strings()
	print possible_rna_strings

