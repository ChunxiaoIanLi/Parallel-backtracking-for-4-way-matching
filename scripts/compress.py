#python ./compress (order) (factor) (input path) (output path)
import os
import sys
import string

order=int(sys.argv[1])
factor=int(sys.argv[2])
inPath=sys.argv[3]
outPath=sys.argv[4]

if not os.path.exists(outPath):
    os.mkdir(outPath)

k=order/factor
case=0
#for each case
while os.path.exists(inPath+"/"+str(order)+"."+str(case)+".A.seqns.txt"):
    print("case: "+str(case))
    if not os.path.exists(outPath+"/"+str(order)+"."+str(case)):
        os.mkdir(outPath+"/"+str(order)+"."+str(case))
    
    outline=""
    outfile="m_"
    tempsum=0
    print("A")
    #A
    if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/A"):
        os.mkdir(outPath+"/"+str(order)+"."+str(case)+"/A")
    infp=open(inPath+"/"+str(order)+"."+str(case)+".A.seqns.txt","r")
    compressfp=open(outPath+"/"+str(order)+"."+str(case)+".A.seqns.txt","a+")
    originalseqn=infp.readline().strip()
    seqn=originalseqn.split(" ")
    while len(seqn)!=1:
        for i in range(k):
            for j in range(factor):
                tempsum+=int(seqn[i+j*k])
            outline+=str(tempsum)+" "
            outfile+=str(tempsum)+"_"
            tempsum=0
        #if no mapping yet, append compressed seqn to compressedfp
        if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/A/"+outfile):
            compressfp.write(outline+'\n')
        mapfp=open(outPath+"/"+str(order)+"."+str(case)+"/A/"+outfile,"a+")
        outline=""
        outfile="m_"
        mapfp.write(originalseqn+'\n')
        mapfp.close()
        originalseqn=infp.readline().strip()
        seqn=originalseqn.split(" ")
    compressfp.close()
    infp.close()
    
    print("B")
    #B
    if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/B"):
        os.mkdir(outPath+"/"+str(order)+"."+str(case)+"/B")
    infp=open(inPath+"/"+str(order)+"."+str(case)+".B.seqns.txt","r")
    compressfp=open(outPath+"/"+str(order)+"."+str(case)+".B.seqns.txt","a+")
    originalseqn=infp.readline().strip()
    seqn=originalseqn.split(" ")
    while len(seqn)!=1:
        for i in range(k):
            for j in range(factor):
                tempsum+=int(seqn[i+j*k])
            outline+=str(tempsum)+" "
            outfile+=str(tempsum)+"_"
            tempsum=0
        #if no mapping yet, append compressed seqn to compressedfp
        if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/B/"+outfile):
            compressfp.write(outline+'\n')
        mapfp=open(outPath+"/"+str(order)+"."+str(case)+"/B/"+outfile,"a+")
        outline=""
        outfile="m_"
        mapfp.write(originalseqn+'\n')
        mapfp.close()
        originalseqn=infp.readline().strip()
        seqn=originalseqn.split(" ")
    compressfp.close()
    infp.close()
    
    print("C")
    #C
    if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/C"):
        os.mkdir(outPath+"/"+str(order)+"."+str(case)+"/C")
    infp=open(inPath+"/"+str(order)+"."+str(case)+".C.seqns.txt","r")
    compressfp=open(outPath+"/"+str(order)+"."+str(case)+".C.seqns.txt","a+")
    originalseqn=infp.readline().strip()
    seqn=originalseqn.split(" ")
    while len(seqn)!=1:
        for i in range(k):
            for j in range(factor):
                tempsum+=int(seqn[i+j*k])
            outline+=str(tempsum)+" "
            outfile+=str(tempsum)+"_"
            tempsum=0
        #if no mapping yet, append compressed seqn to compressedfp
        if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/C/"+outfile):
            compressfp.write(outline+'\n')
        mapfp=open(outPath+"/"+str(order)+"."+str(case)+"/C/"+outfile,"a+")
        outline=""
        outfile="m_"
        mapfp.write(originalseqn+'\n')
        mapfp.close()
        originalseqn=infp.readline().strip()
        seqn=originalseqn.split(" ")
    compressfp.close()
    infp.close()

    print("D")
    #D
    if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/D"):
        os.mkdir(outPath+"/"+str(order)+"."+str(case)+"/D")
    infp=open(inPath+"/"+str(order)+"."+str(case)+".D.seqns.txt","r")
    compressfp=open(outPath+"/"+str(order)+"."+str(case)+".D.seqns.txt","a+")
    originalseqn=infp.readline().strip()
    seqn=originalseqn.split(" ")
    while len(seqn)!=1:
        for i in range(k):
            for j in range(factor):
                tempsum+=int(seqn[i+j*k])
            outline+=str(tempsum)+" "
            outfile+=str(tempsum)+"_"
            tempsum=0
        #if no mapping yet, append compressed seqn to compressedfp
        if not os.path.exists(outPath+"/"+str(order)+"."+str(case)+"/D/"+outfile):
            compressfp.write(outline+'\n')
        mapfp=open(outPath+"/"+str(order)+"."+str(case)+"/D/"+outfile,"a+")
        outline=""
        outfile="m_"
        mapfp.write(originalseqn+'\n')
        mapfp.close()
        originalseqn=infp.readline().strip()
        seqn=originalseqn.split(" ")
    compressfp.close()
    infp.close()
    
    case+=1
