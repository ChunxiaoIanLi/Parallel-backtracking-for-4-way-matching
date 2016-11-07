#!/bin/bash

#outpath: /scratch/ianli/36to45/sequence/
#inpath: /scratch/ianli/output/

outpath=$1
inpath=$2

for file in $(ls $outpath);
do
	cat $inpath$file | grep "finish" >> $outpath$file
done;
