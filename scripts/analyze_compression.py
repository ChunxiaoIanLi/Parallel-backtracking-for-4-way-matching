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
#for each output
for log in logs:
    sp = string.split(log,'.')
    n=sp[0]
    case=sp[1]
    compression=sp[2]

    if not os.path.exists(outputpath+n+"."+case+"."+compression):
        print("n: {0} c: {1} compression: {2}".format(n,case, compression))
        A=[]
        with open(rootpath+n+"."+case+"."+compression+"."+"A.seqns.txt") as seqfp:
            for line in seqfp:
                A.append(line.strip('\n'))
        B=[]
        with open(rootpath+n+"."+case+"."+compression+"."+"B.seqns.txt") as seqfp:
            for line in seqfp:
                B.append(line.strip('\n'))
        C=[]
        with open(rootpath+n+"."+case+"."+compression+"."+"C.seqns.txt") as seqfp:
            for line in seqfp:
                C.append(line.strip('\n'))
        D=[]
        with open(rootpath+n+"."+case+"."+compression+"."+"D.seqns.txt") as seqfp:
            for line in seqfp:
                D.append(line.strip('\n'))
        #chdir to folder containing output from all processors
        currentpath+=log
        os.chdir(currentpath)
        outfp= open(outputpath+n+"."+case+"."+compression, "a+")
        for sublog in os.listdir(currentpath):
            with open(sublog) as infp:
                for line in infp:
                    sp = string.split(line, " ")
                    a=int(sp[1])
                    b=int(sp[2])
                    c=int(sp[3])
                    d=int(sp[4])
                    linetowrite=""
                    linetowrite+=A[a-1]
                    linetowrite+=B[b-1]
                    linetowrite+=C[c-1]
                    linetowrite+=D[d-1]
                    linetowrite+='\n'
                    outfp.write(linetowrite)
        os.chdir("../")
        currentpath=os.getcwd()
        currentpath+="/"
        outfp.close()
