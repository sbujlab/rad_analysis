#!/bin/bash
#
cd /home/cameronc/gitdir/remoll/build/output
read -p 'Folder: ' mod
i=1
while [ $i -le 10 ];
do
  hadd out_${mod}${i}/remoll_${mod}_1M_int1.root out_${mod}${i}/remoll_${mod}_1M_1.root out_${mod}${i}/remoll_${mod}_1M_2.root  
  rm out_${mod}${i}/remoll_${mod}_1M_1.root
  rm out_${mod}${i}/remoll_${mod}_1M_2.root
  mv out_${mod}${i}/remoll_${mod}_1M_int1.root out_${mod}${i}/remoll_${mod}_1M_1.root
  hadd out_${mod}${i}/remoll_${mod}_1M_int2.root out_${mod}${i}/remoll_${mod}_1M_1.root out_${mod}${i}/remoll_${mod}_1M.root 
  rm out_${mod}${i}/remoll_${mod}_1M_1.root
  rm out_${mod}${i}/remoll_${mod}_1M.root
  mv out_${mod}${i}/remoll_${mod}_1M_int2.root out_${mod}${i}/remoll_${mod}_1M.root
done
