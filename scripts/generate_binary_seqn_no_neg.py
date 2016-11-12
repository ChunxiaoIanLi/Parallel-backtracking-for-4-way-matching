#python generate_binary_seqn.py order outfile
import os
import sys
import math

order=int(sys.argv[1])
outfile=sys.argv[2]

k=int(math.ceil((order-1)/2.0))

if(os.path.exists(outfile)):
    outfp=open(outfile, "w")
else:
    outfp=open(outfile, "a+")

def bin_recurs(halflist, depth):
    if depth==k:
        line="1 "
        for i in halflist:
            line=line+i+" "
        #if order is even
        if order%2==0:
            for i in range(1, k):
                line=line+halflist[k-i-1]+" "
        else:
            for i in range(k):
                line+=halflist[k-i-1]+" "
        line+="\n"
        outfp.write(line)
#        line="-1 "+line[2:]
 #       outfp.write(line)
    else:
        halflist[depth]="1"
        bin_recurs(halflist, depth+1)
        halflist[depth]="-1"
        bin_recurs(halflist, depth+1)
    return

firsthalf=["1"]*k
depth=0
bin_recurs(firsthalf, depth)

outfp.close()

