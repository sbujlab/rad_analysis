read -p 'Modifier for analysis: ' mod
DISPLAY=NULL
cd ${mod}_10M/
mkdir Plots_${mod}_10M
cd Plots_${mod}_10M
ls ../out_${mod}*/*101.root >> ${mod}.txt
cp ../libremoll.so .
cp ../rad_dose .
cp ../remolltypes.hh .
./rad_dose ${mod}.txt 
