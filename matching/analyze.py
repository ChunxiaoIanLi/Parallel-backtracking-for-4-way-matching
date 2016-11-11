import os
import sys
import string

#usage: python (analyze.py) (root folder for paf and seq files, make sure to end with "/")

rootpath=sys.argv[1]
outputpath=rootpath+"sequences/"
currentpath=rootpath

#navigate to rootfolder containing pafs and seqs
os.chdir(rootpath)
if not os.path.exists("sequences"):
    os.mkdir("sequences")

currentpath+="output/"
logs=os.listdir(currentpath)
for log in logs:
    sp = string.split(log,'.')
    n=sp[0]
    case=sp[1]
    print("n: {0} c: {1}".format(n,case))
    
    #chdir to folder containing output from all processors
    currentpath+=log
    os.chdir(currentpath)
    outfp= open(outputpath+n+"."+case, "a+")
    for sublog in os.listdir(currentpath):
        with open(sublog) as infp:
            for line in infp:
                sp = string.split(line, " ")
                a=int(sp[1])
                b=int(sp[2])
                c=int(sp[3])
                d=int(sp[4])
                linetowrite=""
                with open(rootpath+n+"."+case+"."+"A.seqns.txt") as seqfp:
                    for i, line in enumerate(seqfp):
                        if(i==a-1):
                            linetowrite+=line.strip("\n")
                            break;
                with open(rootpath+n+"."+case+"."+"B.seqns.txt") as seqfp:
                    for i, line in enumerate(seqfp):
                        if(i==b-1):
                            linetowrite+=line.strip("\n")
                            break;
                with open(rootpath+n+"."+case+"."+"C.seqns.txt") as seqfp:
                    for i, line in enumerate(seqfp):
                        if(i==c-1):
                            linetowrite+=line.strip("\n")
                            break;
                with open(rootpath+n+"."+case+"."+"D.seqns.txt") as seqfp:
                    for i, line in enumerate(seqfp):
                        if(i==d-1):
                            linetowrite+=line
                            break;
                outfp.write(linetowrite)
    os.chdir("../")
    currentpath=os.getcwd()
    currentpath+="/"
    outfp.close()
