#!/bin/bash
#
dir=$HOME/gitdir/remoll/build/output
dirMv=$HOME/gitdir/remoll/output

i=1
read -p 'Filename Modifier: ' mod
read -p 'Number of folders: ' j
mkdir $dirMv/${mod}_${j}M
mkdir $dirMv/Plots_${mod}_${j}M
while [ $i -le $j ];
do
  #destination, add in, add original
	mv $dir/out_${mod}$i $dirMv/${mod}_${j}M/
	let i=i+1
done
export DISPLAY=""
/home/cameronc/gitdir/remoll/rad_analysis/rad_dose ${mod}
sleep 5
export DISPLAY="localhost:12.0"
