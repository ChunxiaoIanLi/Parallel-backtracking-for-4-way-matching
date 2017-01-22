#!/bin/bash

path=$1 #source file path
start=$2 #starting order
end=$3 #ending order
r=$4 #runtime
n=$5 #number of cores

if [ ! -d $path../output_onesol ];
then
    mkdir $path../output_onesol
fi

for order in $(seq $start $end);
do
	if [ $((order%2)) -eq 0 ];
	# even case
	then
		case=0
			while [ -e $path$order.$case.A.pafs.txt ];
			do
				#sqsub -r $r -q mpi -n 50 -o $path../output_onesol/$order.$case /home/ianli/matching/mpi/onesolution.out 0 $path$order.$case.A.pafs.txt $path$order.$case.B.pafs.txt $path$order.$case.C.pafs.txt $path$order.$case.D.pafs.txt echo
				((case++))
			done
	# odd case
	else
		case=0
            while [ -e $path$order.$case.A.pafs.txt ];
            do
                sqsub -r $r -q mpi -n 50 -o $path../output_onesol/$order.$case /home/ianli/matching/mpi/onesolution.out 0 $path$order.$case.A.pafs.txt $path$order.$case.B.pafs.txt $path$order.$case.C.pafs.txt $path$order.$case.D.pafs.txt echo
                ((case++))
            done
	fi
done


#sqsub -r 1d -q mpi -n 50 -o $path\output/$n.$c /home/ianli/matching/mpi/a.out 0 $path$n.$c.A.pafs.txt $path$n.$c.B.pafs.txt $path$n.$c.C.pafs.txt $path$n.$c.D.pafs.txt

: '
for file in $(ls $path);
do
	for case in $(echo $file | grep "A" | grep "pafs");
	do
		IFS='.' read -a myarray <<EOF
< "$case"

n=${myarray[0]}
c=${myarray[1]}
sqsub -r 1d -q mpi -n 50 -o $path\output/$n.$c /home/ianli/matching/mpi/a.out 0 $path$n.$c.A.pafs.txt $path$n.$c.B.pafs.txt $path$n.$c.C.pafs.txt $path$n.$c.D.pafs.txt
done;
done;
'
