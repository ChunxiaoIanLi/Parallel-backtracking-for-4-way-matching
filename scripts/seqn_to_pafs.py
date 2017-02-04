#python seqn_to_pafs.py (input file) (output file)

import os
import sys
import math

seqnfile=sys.argv[1]
temp=seqnfile.split(".")
paffile=sys.argv[2]

seqnfp=open(seqnfile,"r")
paffp=open(paffile,"w")
seqn=seqnfp.readline().strip().split(" ")
k=len(seqn)
s_range=int(math.ceil((k-1)/2.0))

while len(seqn)!=1:
    line=""
    for s in range(1, s_range+1):
        temp=0
        for i in range(k):
            temp+=int(seqn[i])*int(seqn[(i+s)%k])
        line+=str(temp)
        if s!=s_range:
            line+=" "
    line+="\n"
    paffp.write(line)
    seqn=seqnfp.readline().strip().split(" ")

seqnfp.close()
paffp.close()
