#!/bin/bash    

i=$1
end=$2


while [ $i -le $end ]
do
    sqkill $i
i=$[$i+1]

done
