#!/bin/bash

#./matching.sh (lambda) (k) (path to the parent of different cases) (executable path)

lambda=$1
k=$2
executable=$4
if [ -d "$3" ] ; then
    #prefix of the path
    path=$3
    #if prefix didn't end wih a '/', add it
    if [ ! ${path: -1} = '/' ]; then
	path=$path/
    fi
    mkdir kraken_output/$path
    #iterate through each sub case
    for case in $(ls $path); do
	files=""
	#adding each file's path as an argument
	for file in $(ls "$path$case"); do
	    #if [ $file == "26s.nor" ];then
	#	files=$files" "$path$case/$file
	 #   fi

	    files=$files" "$path$case/$file
	done
	#submit job for current case
      	sqsub -r 7d --mpp=1g -o kraken_output/$path$case.log $executable $lambda $k $files
    done

else
    echo no such directory exists
fi