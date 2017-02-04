#!/bin/bash

path=$1
start=$2 #starting order
end=$3 #ending order
r=$4

for order in $(seq $start $end);
do
    case=0
        while [ -e $path$order.$case.A.pafs.txt ];
        do
            #serial
            sqsub -r $r -o $path../col/$order.$case /home/ianli/matching/serial/a.out 0 $path$order.$case.A.pafs.txt $path$order.$case.B.pafs.txt $path$order.$case.C.pafs.txt $path$order.$case.D.pafs.txt
            sqsub -r $r -o $path../col/$order.$case.col /home/ianli/matching/serial/colprio.out 0 $path$order.$case.A.pafs.txt $path$order.$case.B.pafs.txt $path$order.$case.C.pafs.txt $path$order.$case.D.pafs.txt
            ((case++))
            done
done

