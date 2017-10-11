import pdb 
def ps_match(x,y):
        l = len(y);
        while (l > 0):
                if (x.endswith(y[:l])):
                        break
                else:
			#pdb.set_trace()
                        l -= 1
        return (l) # as the increment is post comparison 

if __name__=="__main__":
	l = ps_match("ATTAGTCCTGA","ACCTGACCGGAA") 
	print l
