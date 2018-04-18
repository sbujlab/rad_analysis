#!/bin/bash
#
location=$HOME/gitdir/remoll/build/output/
cd $location

read -p 'Modifier: ' mod
read -p 'Number of jobs: ' j
i=1

for i in `seq 1 $j`;
do
	name="out_${mod}${i}"
	mkdir $name
	cd $name
	qsub ../../macros/runscript_${mod}.sh
  sleep 1
  cd ..
done
