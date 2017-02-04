import os
import sys
import string
rootpath=sys.argv[1]
checkpath=rootpath+"output/"

files=os.listdir(rootpath)
for f in files:
    sp = string.split(f,'.')
    n=sp[0]
   
    if n != "output" and n != "sequences":
        case=sp[1]        
        if not os.path.exists(checkpath+n+"."+case):
            print("n: {0} c: {1} is missing".format(n,case))
    

