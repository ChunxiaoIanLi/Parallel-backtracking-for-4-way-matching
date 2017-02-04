import os
import sys
import string

#usage: python (analyze.py) (root folder for paf and seq files, make sure to end with "/")

rootpath=sys.argv[1]
outputpath="/scratch/ianli/golay_seq/"
currentpath="/scratch/ianli/output_golay/"

#navigate to rootfolder containing pafs and seqs
os.chdir(rootpath)

logs=os.listdir(currentpath)
os.chdir(currentpath)

logs=['24']
for n in logs:
    print("n: {0}".format(n))
    
    #chdir to folder containing output from all processor
    outfp= open(outputpath+n, "a+")
    
    with open(n) as infp:
        for line in infp:
            sp = string.split(line, " ")
            a=int(sp[1])
            b=int(sp[2])
            
            linetowrite=""
            with open(rootpath+n+"."+"A.seqns.txt") as seqfp:
                for i, line in enumerate(seqfp):
                    if(i==a-1):
                        linetowrite+=line.strip("\n")
                        break;
            linetowrite+=" "
            with open(rootpath+n+"."+"B.seqns.txt") as seqfp:
                for i, line in enumerate(seqfp):
                    if(i==b-1):
                        linetowrite+=line.strip("\n")
                        break;
            linetowrite+="\n"
            outfp.write(linetowrite)
    outfp.close()
