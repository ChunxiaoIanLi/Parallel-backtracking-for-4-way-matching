import os
import sys
import string
import glob
import subprocess

#usage: python (submit_compression.py) (root folder for paf and seq files)

rootpath=sys.argv[1]

for n in range(0, 45):
    c=0
    while glob.glob(rootpath+"/"+str(n)+"."+str(c)+"*"):
        for compression in range(n/2+1):
            if os.path.exists(rootpath+"/"+str(n)+"."+str(c)+"."+str(compression)+".A.pafs.txt"):
                if compression!=n/2:
                    print("{0}.{1}.{2}".format(n,c,compression))
                    subprocess.call(["sqsub","-r","10m","-q","mpi","-n","50","-o","/scratch/ianli/output_new_compression/{0}.{1}.{2}".format(n,c,compression), "/home/ianli/matching/mpi/matching_compression.out","0","/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.A.pafs.txt".format(n,c,compression),"/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.B.pafs.txt".format(n,c,compression),"/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.C.pafs.txt".format(n,c,compression),"/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.D.pafs.txt".format(n,c,compression),"tofile"])
                else:
                    print("{0}.{1}.{2}".format(n,c,compression))
                    subprocess.call(["sqsub","-r","10m","-q","mpi","-n","50","-o","/scratch/ianli/output_new_compression/{0}.{1}.{2}".format(n,c,compression), "/home/ianli/matching/mpi/matching_compression.out","0","/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.A.pafs.txt".format(n,c,compression),"/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.B.pafs.txt".format(n,c,compression),"/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.C.pafs.txt".format(n,c,compression),"/scratch/ianli/new_compression_pafs/{0}.{1}.{2}.D.pafs.txt".format(n,c,compression),"tofile"])                    
        c+=1


'''
                with open("/scratch/ianli/output_compression/{0}.{1}.{2}".format(n,c,compression), "w+") as outfile:
                    subprocess.call(["mpirun","-np","10","/home/ianli/matching/mpi/matching_compression.out","0","/scratch/ianli/compression_pafs/{0}.{1}.{2}.A.pafs.txt".format(n,c,compression),"/scratch/ianli/compression_pafs/{0}.{1}.{2}.B.pafs.txt".format(n,c,compression),"/scratch/ianli/compression_pafs/{0}.{1}.{2}.C.pafs.txt".format(n,c,compression),"/scratch/ianli/compression_pafs/{0}.{1}.{2}.D.pafs.txt".format(n,c,compression),"tofile"], stdout=outfile)
'''
