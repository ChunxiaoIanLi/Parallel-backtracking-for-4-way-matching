#!/bin/bash

for file in $(ls * | grep -v ".sh");
do
    IFS='.' read -a myarray <<< "$file"
	
    n=${myarray[0]}
    c=${myarray[1]}
	echo n=$n c=$c
	if grep -q "SOLUTION" $file;
	then
		grep "SOLUTION" $file | while read line;
		do
			IFS=' ' read -a rows <<<"$line"
			rowA=${rows[1]}
			rowB=${rows[2]}
			rowC=${rows[3]}
			rowD=${rows[4]}
			head -n $rowA ../$n.$c.A.seqns.txt | tail -1 >> ../sequence/$n
			head -n $rowB ../$n.$c.B.seqns.txt | tail -1 >> ../sequence/$n
			head -n $rowC ../$n.$c.C.seqns.txt | tail -1 >> ../sequence/$n
			head -n $rowD ../$n.$c.D.seqns.txt | tail -1 >> ../sequence/$n
			echo $'\n'>>../sequence/$n
		done;
	else
		echo None>>../sequence/$n
	fi;
	cat $file | grep "finish the search"
    
done;