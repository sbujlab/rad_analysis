#!/bin/bash
#

############################ 
##### Define Functions #####

#####
# Run jobs

iterator () {
  setBuildDir
  cd $BUILD
  modReader 
  outLocation=$PWD/output/
  cd $outLocation
  initial=1
  
  if [ $changeInclude -eq 1 ];
    then
    read -p 'Number of jobs: ' final
  fi

  if [ $changeInclude -eq 0 ];
    then
    let initial=0
    read -p 'Minimum value of included.xml variable: ' min
    read -p 'Maximum value of included.xml variable: ' max
    read -p 'Step size of included.xml variable: ' step
    let final=$(printf "%.0f\n" "$(bc -l <<< \($max-$min\)/$step)")
  fi 

  for i in `seq $initial $final`;
  do
    name=out_${mod}${i}
    mkdir $name
    cd $name
    mkdir geometry
    cp -rp "${BUILD}"/geometry/*${mod}* "./geometry/"
    cp -rp "${BUILD}"/geometry/materials.xml "./geometry/"
    cp -rp "${BUILD}"/geometry/schema "./geometry/"
    cp -rp "${BUILD}"/remoll "./remoll"
    cp -rp "${BUILD}"/macros/runscript_${mod}.sh "./runscript_${mod}.sh"
    cp -rp "${BUILD}"/macros/runexample_${mod}.mac "./runexample_${mod}.mac"
    if [ $changeInclude -eq 0 ];
      then
      changeIncludes $i $mod $min $max $final
    fi
    qsub ./runscript_${mod}.sh
    sleep 1
    cd ..
  done
}

#####
# Set the current build directory by looking up at most 3 directories (including the current working directory) and searching in each one for a directory named build who position is assigned to a variable $BUILD
setBuildDir () {

if [ -d ./build ]
  then
    pushd ./build > /dev/null
    BUILD=$PWD
  elif [ -d ../build ]
  then
    pushd ../build > /dev/null
    BUILD=$PWD;
  elif [ -d ../../build ]
  then
    pushd ../../build > /dev/null
    BUILD=$PWD;
  fi
  popd > /dev/null
}

#####
# Create a new included .xml file to be read in by whatever the ${mod} files expect - not intended for standalone command line use - called by iterator only if needed.

changeIncludes () {
  n=$(printf "%.2f\n" "$(bc -l <<< ${3}+\(\(${1}\)*\(${4}-${3}\)/${5}\))") #initial parameter + (iterator parameter-1) * (final - initial parameter limits) / number of steps
  modifier=${2}
  include_name=geometry/include_${modifier}_variable.xml
/bin/cat <<EOM >$include_name
<!-- $include_name include file for ${n} parameter, $((${1}+1)) step of $((${5}+1)), running from ${3} to ${4} -->
EOM

  # Take the number n and use it to print (in ./geometry/) a schematically identical included .xml file that gets placed next to the rest of the gdml files
}

#####
# Create runexample_${mod}.mac file

macroPrinter () {

  setBuildDir
  cd $BUILD
  modReader 

  macroFileName="macros/runexample_${mod}.mac"

/bin/cat <<EOM >$macroFileName
# Macrofile
# This must be called before initialize
/remoll/setgeofile geometry/mollerMother_${mod}.gdml
# This must be explicitly called
/run/initialize
/remoll/addfield $BUILD/map_directory/blockyHybrid_rm_3.0.txt
/remoll/addfield $BUILD/map_directory/blockyUpstream_rm_1.1.txt
# Raster and initial angle stuff
/remoll/oldras false
/remoll/rasx 5 mm
/remoll/rasy 5 mm
/remoll/beamene 11 GeV
/remoll/gen beam
/remoll/beamcurr 85 microampere
# Make interactions with W, Cu, and Pb
# realistic rather than pure absorbers
/remoll/kryptonite false
/process/list
# Specify random number seed - DO NOT USE THE SAME SEED OVER AND OVER AGAIN
#/remoll/seed 123456
/remoll/filename remoll_${mod}_1M.root
/run/beamOn 1000000
EOM
}

#####
# Create runscript_${mod}.sh file

runscriptPrinter () {

  setBuildDir
  cd $BUILD
  modReader 

  qsubFileName="macros/runscript_${mod}.sh"
  
/bin/cat <<EOM >$qsubFileName
#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
./remoll ./runexample_${mod}.mac
EOM
}

#####
# create mollerMother_${mod}.gdml file

motherGDMLPrinter () {

  setBuildDir
  cd $BUILD
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
      <file name="geometry/targetDaughter_${mod}.gdml"/>
      <positionref ref="targetCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="geometry/hallDaughter_${mod}.gdml"/>
      <positionref ref="hallCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="geometry/detectorDaughter_${mod}.gdml"/>
      <positionref ref="detectorCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="geometry/upstreamDaughter_${mod}.gdml"/>
      <positionref ref="upstreamCenter"/>
      <rotationref ref="identity"/>
      </physvol>

      <physvol>
      <file name="geometry/hybridDaughter_${mod}.gdml"/>
      <positionref ref="hybridCenter"/>
      <rotationref ref="identity"/>
      </physvol>    

<!--
      <physvol>
      <file name="geometry/dumpDaughter_${mod}.gdml"/>
      <positionref ref="hybridCenter"/>
      <rotationref ref="identity"/>
      </physvol>    
-->

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
  #changeInclude=0
  #motherGDMLPrinter
  #macroPrinter
  #runscriptPrinter
  iterator
fi

#####
# Read the user's modifier input from the command line, only if the user hasn't already done so or if the input was null

modReader(){
  if [[ $modSet -eq 1 ]] || [[ -z "$mod" ]]
  then
    modSet=0
    read -p 'Modifier for iterator, macro and runscript creation: ' mod
  fi
}

