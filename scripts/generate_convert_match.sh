#!/bin/bash

#
i=$1
end=$2
dir=$3

while [ $i -le $end ]
do
	python generate_binary_seqn.py $i $dir/$i.seqn
	python seqn_to_pafs.py $dir/$i.seqn $dir/$i.pafs 
	/home/ianli/matching/serial/old.out 0 $dir/$i.pafs $dir/$i.pafs $dir/$i.pafs $dir/$i.pafs > $dir/$i.log
	i=$[$i+1]
done