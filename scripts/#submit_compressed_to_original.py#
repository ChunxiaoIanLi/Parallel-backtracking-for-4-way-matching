#python submit_compressed_to_original.py (executable) (runtime) (input file path) (output path) (rootpath)

import os
import sys
import string
import subprocess

inputpath=sys.argv[3]
temp=inputpath.split("/")
filename=temp[-1]
outputpath=sys.argv[4]
runtime=sys.argv[2]
executable=sys.argv[1]
rootpath=sys.argv[5]
counter=0

if not os.path.exists(outputpath):
    os.mkdir(outputpath)
    
matchfp=open(inputpath,"r")
line=matchfp.readline().strip().split(" ")
order=len(line)/4
while len(line)!=1:
    A="m_"
    B="m_"
    C="m_"
    D="m_"
    for i in range(order):
        A+=line[i]+"_"
        B+=line[i+order]+"_"
        C+=line[i+2*order]+"_"
        D+=line[i+3*order]+"_"
    A+=".pafs"
    B+=".pafs"
    C+=".pafs"
    D+=".pafs"
    subprocess.call(["echo","line: {0}".format(counter+1)])
    subprocess.call(["{0}".format(executable),"0","{0}/{1}/A/{2}".format(rootpath, filename, A),"{0}/{1}/B/{2}".format(rootpath, filename, B),"{0}/{1}/C/{2}".format(rootpath, filename, C),"{0}/{1}/D/{2}".format(rootpath, filename, D)])
    #subprocess.call(["sqsub","-r","{0}".format(runtime),"-o","{0}/{1}".format(outputpath, counter),"{0}".format(executable),"0","{0}/{1}/A/{2}".format(rootpath, filename, A),"{0}/{1}/B/{2}".format(rootpath, filename, B),"{0}/{1}/C/{2}".format(rootpath, filename, C),"{0}/{1}/D/{2}".format(rootpath, filename, D)])
      #  print("sqsub -r {0} -o {1}/{2} {3} 0 {4}/{5}/A/{6} {7}/{8}/D/{9} {10}/{11}/D/{12} {13}/{14}/D/{15}".format(runtime, outputpath, counter, executable, rootpath, filename, A, rootpath, filename, B, rootpath, filename, C, rootpath, filename, D))
    counter+=1
    line=matchfp.readline().strip().split(" ")
matchfp.close()
