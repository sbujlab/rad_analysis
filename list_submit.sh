#!/bin/bash
if [ "$#" -eq 1 ]; then
  list=$1
else
  read -p "job input list.txt file name: " list
fi
while read p; do 
  /site/bin/swif add-jsub -workflow remoll -script $p 
done <$list
