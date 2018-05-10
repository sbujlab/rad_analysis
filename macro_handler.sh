#!/bin/bash
#

############################ 
##### Define Functions #####

#####
# Run jobs

iterator () {

  outLocation=$HOME/gitdir/remoll/build/output/
  cd $outLocation
  initial=1
  
  read -p 'Number of jobs: ' j

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
    cp -rp "${HOME}"/gitdir/remoll/geometry/*${mod}* "./geometry/"
    if [ $changeInclude -eq 0 ];
      then
      changeIncludes $i $mod $min $max $final
    fi
  #  qsub ../../macros/runscript_${mod}.sh
    sleep 1
    cd ..
  done
}

#####
# Create a new included .xml file to be read in by whatever the ${mod} files expect

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

  buildLocation=$HOME/gitdir/remoll/build/
  cd $buildLocation

  macroFileName="macros/runexample_${mod}.mac"

/bin/cat <<EOM >$macroFileName
  # Macrofile
  # This must be called before initialize
  /remoll/setgeofile mollerMother_${mod}.gdml #../../geometry/mollerMother_${mod}.gdml
  # This must be explicitly called
  /run/initialize
  /remoll/addfield $buildLocation/map_directory/blockyHybrid_rm_3.0.txt
  /remoll/addfield $buildLocation/map_directory/blockyUpstream_rm_1.1.txt
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

  buildLocation=$HOME/gitdir/remoll/build/
  cd $buildLocation

  qsubFileName="macros/runscript_${mod}.sh"
  
/bin/cat <<EOM >$qsubFileName
  #!/bin/bash
  #
  #$ -cwd
  #$ -j y
  #$ -S /bin/bash
  ../../remoll ../../macros/runexample_${mod}.mac
EOM
}

#####
# create mollerMother_${mod}.gdml file

motherGDMLPrinter () {

  buildLocation=$HOME/gitdir/remoll/build/
  cd $buildLocation

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

if [ $# != 0 ]; then
  read -p 'Modifier: ' mod
  for input in $@; do
    case $input in
      -c|changeIncludes)
        let changeInclude=0
        echo "Be sure to place changeIncludes before iterator flag"
      ;;
      -m|macro)
        macroPrinter
      ;;
      -r|runscript)
        runscriptPrinter
      ;;
      gdml|-g|GDML)
        motherGDMLPrinter  
      ;;
      -s|submit|-i|iterator)
        iterator
        echo "Be sure to place iterator at the end of the list of flags"
      ;;
      *)
        echo "Usage: macro printing, include file iteration, runscript printing, GDML printing, or iterator running"
      ;;
    esac
  #shift
  done
elif [ $# == 0 ]; then
  read -p 'Modifier for iterator, macro and runscript creation: ' mod
  macroPrinter
  runscriptPrinter
  iterator
fi
