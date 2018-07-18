#!/bin/bash
if [ "$#" -eq 1 ]; then
  mod=$1
else
  read -p "job name (merged, etc.): " mod
fi
export LD_PRELOAD=libremoll.so
echo "ls /lustre/expphy/volatile/halla/parity/cameronc/remoll/output/${mod}/moller_${mod}*/*.root > list_${mod}.txt"
ls /lustre/expphy/volatile/halla/parity/cameronc/remoll/output/${mod}/moller_${mod}*/*.root > list_${mod}.txt 
./hallRad --infile list_${mod}.txt
