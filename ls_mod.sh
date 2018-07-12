#!/bin/bash
if [ "$#" -eq 2 ]; then
  mod=$1
  par=$2
  unit="mm"
else
  read -p "job name (merged, etc.): " mod
fi
echo "ls /lustre/expphy/volatile/halla/parity/cameronc/remoll/output/${mod}/moller_${mod}*/*.root > list_${mod}.txt"
ls /lustre/expphy/volatile/halla/parity/cameronc/remoll/output/${mod}/moller_${mod}*/*.root > list_${mod}.txt 
./hallRad --infile list_${mod}.txt
