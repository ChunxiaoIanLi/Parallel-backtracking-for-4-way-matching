import os
import sys
sys.path.append("/home/ianli/matching/scripts/tabulate-0.7.7")
import string
from tabulate import tabulate

#usage: python (analyze.py) (root folder for output files)

def greater(a, b):
    al=a.split(".")
    bl=b.split(".")
    a_o=int(al[0])
    b_o=int(bl[0])
    a_c=int(al[1])
    b_c=int(bl[1])
    if a_o>b_o:
        return 1
    if a_o==b_o and a_c>b_c:
        return 1
    return -1

rootpath=sys.argv[1]
os.chdir(rootpath)
logs=os.listdir(rootpath)
logs.sort(cmp=greater)
#print(logs)

row_format ="{:>15}" * 3

t=[]
for log in logs:
    output=log
    times=[]
    with open(log) as f:
        for line in f:
            if "seconds to finish" in line:
                l = line.strip().split(" ")
                for i in l:
                    if i[0].isdigit():
                        times.append(float(i))
    output+="           "
    if len(times)==0:
        output+="-"
        #t.append([log, "-"])
    else:
        output+=str(min(times))
        #t.append([log, str(min(times))])
        #time=str(min(times))
    print(output)
#print tabulate(t, headers=["order", "time"])

