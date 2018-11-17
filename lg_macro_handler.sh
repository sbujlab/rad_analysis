#!/bin/bash
#

############################ 
##### Define Functions #####

#####
# Run jobs

iterator () {
  setBuildDir
  #cd $BUILD
  modReader 
  outLocation=$PWD
  cd $outLocation
  initial=1
  
  if [ $changeInclude -eq 1 ];
    then
    read -p 'Number of jobs: ' final
  fi

  if [ $changeInclude -eq 0 ];
    then
    let initial=0
    read -p 'Minimum value of variable: ' min
    read -p 'Maximum value of variable: ' max
    read -p 'Step size of variable: ' step
    let final=$(printf "%.0f\n" "$(bc -l <<< \($max-$min\)/$step)")
    echo "Angle = "
    echo $final
  fi 

  for i in `seq $initial $final`;
  do
    name=out_${mod}_${i}deg
    mkdir output/$name
    mkdir output/$name/geometry
    mkdir output/$name/geometry/schema
    macroPrinter ${i}
    runscriptPrinter ${i}
    cp build/remoll output/out_${mod}_${i}deg/
    cp build/libremoll.so output/out_${mod}_${i}deg/
    cp analysis/build/pe output/out_${mod}_${i}deg/
    cp analysis/build/libpe.so output/out_${mod}_${i}deg/
    #cp rad_analysis/pruneTreeLGtest output/out_${mod}_${i}deg/ # This is useful for pruning a normal run to only have det 28 hit info in it for use in the external generator
    cp schema/* output/out_${mod}_${i}deg/geometry/schema/
    cp detector/mollerParallel.gdml output/out_${mod}_${i}deg/geometry/
    cp detector/materialsOptical.xml output/out_${mod}_${i}deg/geometry/
    cp detector/detectorMotherP.csv output/out_${mod}_${i}deg/geometry/
    cp detector/cadGeneratorV1.pl output/out_${mod}_${i}deg/geometry/
    cp detector/gdmlGeneratorV1_materials.pl output/out_${mod}_${i}deg/geometry/
    cp detector/qe.txt output/out_${mod}_${i}deg/geometry/
    cp detector/UVS_45total.txt output/out_${mod}_${i}deg/geometry/
    cp detector/MylarRef.txt output/out_${mod}_${i}deg/geometry/
    if [ $changeInclude -eq 0 ];
      then
      #changeIncludes $i $mod $min $max $final
      perlPrinter $i $mod $min $max $final
    fi
    cd output/out_${mod}_${i}deg
    qsub runscript_${mod}_${i}deg.sh
    sleep 1
    cd ../../
  done
}


#####
# Read the user's modifier input from the command line, only if the user hasn't already done so or if the input was null

modReader(){
  if [[ $modSet -eq 1 ]] || [[ -z "$mod" ]]
  then
    modSet=0
    read -p 'Modifier for iterator, macro and runscript creation: ' mod
  fi
}

#####
# Set the current build directory by looking up at most 3 directories (including the current working directory) and searching in each one for a directory named build who position is assigned to a variable $BUILD
setBuildDir () {

#if [ -d ./build ]
#  then
#    pushd ./build > /dev/null
#    BUILD=$PWD
#    popd > /dev/null
#  elif [ -d ../build ]
#  then
#    pushd ../build > /dev/null
#    BUILD=$PWD;
#    popd > /dev/null
#  elif [ -d ../../build ]
#  then
#    pushd ../../build > /dev/null
#    BUILD=$PWD;
#    popd > /dev/null
#  fi
  BUILD=$PWD;
}

#####
# Create a new included .xml file to be read in by whatever the ${mod} files expect - not intended for standalone command line use - called by iterator only if needed.

changeIncludes () {
  n=$(printf "%.2f\n" "$(bc -l <<< ${3}+\(\(${1}\)*\(${4}-${3}\)/${5}\))") #initial parameter + (iterator parameter-1) * (final - initial parameter limits) / number of steps
  perlPrinter $1 $n

#/bin/cat <<EOM >$include_name
#<!-- $include_name include file for ${n} parameter, $((${1}+1)) step of $((${5}+1)), running from ${3} to ${4} -->
#EOM

  # Take the number n and use it to print (in ./geometry/) a schematically identical included .xml file that gets placed next to the rest of the gdml files
}

#####
# Create runexample_${mod}.mac file

macroPrinter () {

  setBuildDir
  #cd $BUILD
  modReader 

  macroFileName="output/out_${mod}_${1}deg/runexample_${mod}_${1}deg.mac"

/bin/cat <<EOM >$macroFileName

#Shoots the backtraced moller hit distribution for the first septant at a single detector

# This must be called before initialize
/remoll/setgeofile geometry/mollerMother_${mod}_${1}deg.gdml

/remoll/physlist/optical/enable 
# This must be explicitly called
/run/initialize

/process/list
/remoll/printgeometry true
/remoll/beamene 11 GeV 
#/remoll/beamene 855 MeV

# Blackening
#/remoll/evgen/set blackening 
#/remoll/evgen/blackening/useFirstSeptant true
#/run/beamOn 0
#/remoll/filename remollout_r5o_blackening_default.root
#/run/beamOn 100

# Beam
/remoll/evgen/set beam
/remoll/evgen/beam/x -987.5
/remoll/evgen/beam/z -20.0
/remoll/evgen/beam/px -0.0523
/remoll/evgen/beam/pz 0.9986 
/remoll/filename remollout_r5o_beam_${mod}_${1}deg.root
/run/beamOn 1000 

# External
/remoll/evgen/set external
/remoll/evgen/external/file ../../remoll_moller_envelope_noCol4_noCoils_500k_LGtest_det28.root
/remoll/evgen/external/detid 28
/remoll/evgen/external/startEvent 0
/remoll/evgen/external/zOffset -28550.0
/remoll/filename remollout_r5o_external_${mod}_${1}deg.root
/run/beamOn 1000
EOM
}

##### 
# Execute perl scripts

perlPrinter() { 
  setBuildDir
  #cd $BUILD
  modReader
  cd output/out_${mod}_${1}deg/geometry/
  
  n=$(printf "%.2f\n" "$(bc -l <<< ${3}+\(\(${1}\)*\(${4}-${3}\)/${5}\))") #initial parameter + (iterator parameter-1) * (final - initial parameter limits) / number of steps
#5.0,987.5,105,1,10,3,90,80,11.5,6,1350,3,6,3,17.33,17.33,-120,0.01
/bin/cat <<EOM >cadp_${mod}_${1}deg.csv
5.0,987.5,105,1,10,3,90,80,$n,6,1350,3,6,3,17.33,17.33,-120,0.01
EOM
  perl cadGeneratorV1.pl -F cadp_${mod}_${1}deg.csv
  perl gdmlGeneratorV1_materials.pl -M detectorMotherP.csv -D parameter.csv -P qe.txt -U UVS_45total.txt -R MylarRef.txt -L 5open -T _${mod}_${1}deg
 cd ../../../
}

#####
# Create runscript_${mod}.sh file

runscriptPrinter () {

  setBuildDir
  #cd $BUILD
  modReader 

  qsubFileName="output/out_${mod}_${1}deg/runscript_${mod}_${1}deg.sh"
  
/bin/cat <<EOM >$qsubFileName
#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
./remoll -t 1 -m ./runexample_${mod}_${1}deg.mac
./pe remollout_r5o_external_${mod}_${1}deg.root 50001
EOM
}

#####
# create mollerMother_${mod}.gdml file

motherGDMLPrinter () {

  setBuildDir
  #cd $BUILD
  modReader 
  motherFileName="geometry/mollerMother_${mod}.gdml"
                                        # Be prepared to remove the ${mod}s from the superfluous sub files each time
                                        # Hard code the ones hat I know work for each application.
/bin/cat <<EOM >$motherFileName
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<gdml xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="schema/gdml.xsd">

<define> 
  <position name="hallCenter" x="0" y="0" z="4000"/>
  <position name="targetCenter" x="0" y="0" z="0"/>
  <position name="upstreamCenter" x="0" y="0" z="7000."/>
  <position name="hybridCenter" x="0" y="0" z="13366.57"/>
  <position name="detectorCenter" x="0" y="0" z="28500."/>
  <position name="dumpCenter" x="0" y="0" z="28500."/>
  <rotation name="identity"/>
</define>

<materials>
     <material Z="1" name="Vacuum" state="gas">
       <T unit="K" value="2.73"/>
       <P unit="pascal" value="3e-18"/>
       <D unit="g/cm3" value="1e-25"/>
       <atom unit="g/mole" value="1.01"/>
     </material>
</materials>

<solids>
     <box lunit="mm" name="boxMother" x="200000" y="200000" z="200000"/>
</solids>

  <structure>

    <volume name="logicMother">
      <materialref ref="Vacuum"/>
      <solidref ref="boxMother"/>

      <physvol>
      <file name="targetDaughter_${mod}.gdml"/>
      <positionref ref="targetCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="hallDaughter_${mod}.gdml"/>
      <positionref ref="hallCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="detectorDaughter_${mod}.gdml"/>
      <positionref ref="detectorCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="upstreamDaughter_${mod}.gdml"/>
      <positionref ref="upstreamCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="hybridDaughter_${mod}.gdml"/>
      <positionref ref="hybridCenter"/>
      <rotationref ref="identity"/>
      </physvol>    

    </volume>
  </structure>

  <setup name="Default" version="1.0">
    <world ref="logicMother"/>
  </setup>
 
</gdml> 
EOM
}


########################
##### Main Program #####

changeInclude=1
modSet=1

if [ $# != 0 ]; then
  read -p 'Modifier: ' mod
  for input in $@; do
    case $input in
      -c|--changeIncludes)
        let changeInclude=0
        echo "Be sure to place changeIncludes before iterator flag"
      ;;
      -m|--macro)
        macroPrinter
      ;;
      -r|--runscript)
        runscriptPrinter
      ;;
      -g|--gdml|--GDML)
        motherGDMLPrinter  
      ;;
      -i|--submit|-s|--iterator)
        iterator
        echo "Be sure to place iterator at the end of the list of flags"
      ;;
      --help|-h|help|*)
        echo "Usage: Execute from a folder either containing, or one or two directories below a folder containing the 'build' directory you intend to run from

Usage: -m macro printing, -c include file printing (not standalone), -r runscript printing, -g GDML printing, or -i iterator running"
      ;;
    esac
  #shift
  done
elif [ $# == 0 ]; then
  #read -p 'Modifier for iterator, macro and runscript creation: ' mod
  changeInclude=0
  #motherGDMLPrinter
  #macroPrinter
  #runscriptPrinter
  #modReader
  iterator
fi
