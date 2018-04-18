#!/bin/bash
#
outLocation=$HOME/gitdir/remoll/build/output/
cd $outLocation

read -p 'Modifier: ' mod
read -p 'Number of jobs: ' j
i=1


# I want to create the runscript.sh file, the runexample.mac file, and maybe even the mollerMother.gdml file all in one go.
# Create functions that will print such files if called, then call them, maybe with case flags to turn them on/off based on default settings of how I feel at the time


for i in `seq 1 $j`;
do
	name="out_${mod}${i}"
	mkdir $name
	cd $name
	qsub ../../macros/runscript_${mod}.sh
  sleep 1
  cd ..
done

#####
# Create runexample_${mod}.mac file

  buildLocation=$HOME/gitdir/remoll/build/
  cd $buildLocation

  macroFileName="macros/runexample_${mod}.mac"


  # Macrofile
  # This must be called before initialize
  /remoll/setgeofile ../../geometry/mollerMother_${mod}.gdml
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

#####
# Create runscript_${mod}.sh file

  buildLocation=$HOME/gitdir/remoll/build/
  cd $buildLocation

  qsubFileName="macros/runscript_${mod}.sh"
  
  #!/bin/bash
  #
  #$ -cwd
  #$ -j y
  #$ -S /bin/bash
  ../../remoll ../../macros/runexample_${mod}.mac

#####
# create mollerMother_${mod}.gdml file

  buildLocation=$HOME/gitdir/remoll/build/
  cd $buildLocation

  motherFileName="geometry/mollerMother_${mod}.gdml"

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
        <file name="../../geometry/targetDaughter_merged.gdml"/>
        <positionref ref="targetCenter"/>
        <rotationref ref="identity"/>
        </physvol>


        <physvol>
        <file name="../../geometry/hallDaughter_merged.gdml"/>
        <positionref ref="hallCenter"/>
        <rotationref ref="identity"/>
        </physvol>

        <physvol>
        <file name="../../geometry/detectorDaughter_merged.gdml"/>
        <positionref ref="detectorCenter"/>
        <rotationref ref="identity"/>
        </physvol>

        <physvol>
        <file name="../../geometry/upstreamDaughter_merged.gdml"/>
        <positionref ref="upstreamCenter"/>
        <rotationref ref="identity"/>
        </physvol>

        <physvol>
        <file name="../../geometry/hybridDaughter_merged.gdml"/>
        <positionref ref="hybridCenter"/>
        <rotationref ref="identity"/>
        </physvol>    

  <!--
        <physvol>
        <file name="../../geometry/dumpDaughter.gdml"/>
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
