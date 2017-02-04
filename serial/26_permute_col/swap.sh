#!/bin/bash

#in=$1
#out=$2
#col1=$3
#col2=$4

#echo awk -v input="$in" -v output="$out" -v c1="$col1" -v c2="$col2" ' { t = c1; c1 = c2; c2 = t; print; } '


#variable="line one\nline two"
#awk -v var="$variable" 'BEGIN {print var}'
awk ' { t = $4; $4 = $7; $7 = t; print; } ' tA > ttA
awk ' { t = $4; $4 = $7; $7 = t; print; } ' tB > ttB
awk ' { t = $4; $4 = $7; $7 = t; print; } ' tC > ttC
awk ' { t = $4; $4 = $7; $7 = t; print; } ' tD > ttD

mv ttA tA
mv ttB tB
mv ttC tC
mv ttD tD
