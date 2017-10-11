#!/usr/bin/env python
import itertools

#prefix-suffix match
def ps_match(x, y):
        merged_str = ""
        l = 0
        max_l = 0
        #print x
        #print y
        while (l < len(y)):
                if x.endswith(y[:l]):
                        #print "suffix x, prefix y"
                        l += 1
                else:
                        break

        max_l = l-1
        l=0
        while (l < len(x)):
                if y.endswith(x[:l]):
                        #print "suffix y, prefix x"
                        l += 1
                else:
                        break

        if l > max_l : # suffix from y matches prefix from x
                max_l = l-1
                merged_str = y+x[max_l:]
        else:           # suffix from x matches prefix from y
                merged_str = x+y[max_l:]
        return max_l, merged_str


if __name__=="__main__":
	l = ["AAACCTTGG","TTACAAG","CTTTACCCGGGA","TTTACCGGAGA","TTACGATTACC","TTAAAAGGCCC"]
	pairs = itertools.combinations(l,2)
	for s1,s2 in pairs:
	        l,s = ps_match(s1,s2)
		print s1,s2	
		print l, s
