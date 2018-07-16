# PREX simulation package

## Getting the code
If you just want to run the code:
 ```
 git clone https://github.com/cipriangal/prexSim
 ```
If you want to commit changes directly to this repository (and have permissions to do so):
 ```
 git clone git@github.com:cipriangal/prexSim
 ```

### Workflow
If you intend to make changes to the code and contribute it is suggested that you fork this repository into your own github account. (There is a button for that on the top right of the page). You will then have a backup for your local repository on github. To get your own repository run the same line as above but change "cipriangal" to your username.

Before you start working on the code make sure you update your local repository from the remote:
 ```
 git pull
 ```
After you are done editing a file (or at the end of the day) you should:
```
git commit -m "clear message of changes" changedFile1.ext ... changedFileN.ext
``` 
(this saves the instance of these files into your local copy of the repository)
```
git push
``` 
(this takes all the saved instances you have in your local repository and send them to the remote repository)

If you have a feature in your fork and you would like to make it available to other you can create a pull request. https://help.github.com/articles/creating-a-pull-request/

This repository contains multiple branches for working on different issues. You can see the list of branches at the top of the file list. If you want to create your own branch follow instrucations at https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging 

### Compiling the code
This code has been known to compile with G4 version up to 4.10.2.3.	
 ```
 mkdir build
 cd build
 cmake ../
 make
 cd -
 ```
### Running the code
 `build/prexsim macros/prex_prerunCiprian.mac` (to visualize)
 
 `build/prexsim macros/prex_prerunCiprian.mac macros/myRun.mac` (for batch mode)

These two .mac files need to be modified in order to run exactly what you want. Make sure you change the number of detectors when changing from prex1 to prex2 configurations.

### Analyzing output

#### anaRad
 ```
 mkdir output
 build/anaRad 
 ```
 
The last line will give you instructions on what is needed as input for the executable. This will produce tables of flux and power in each detector separated by energy. The output rootfile will have additional granularity.

#### Dose estimates
 `build/hallRad`
 
The last line will give you instructions on what is needed as input for the executable.

This analysis executable will run over sensitive detectors and will calculate a 1MeV neutron equivalent and a radiation dose rate measured in mili-rem. A series of detectors are hardcoded as being kryptonite ( [10008,100013], 10101, 10102 ) while the rest are assumed to be vacuum. This is important because the energy being used in the routine to estimate the radiation damage/dose will be either the deposited energy (kryptonite) or the kinetic energy (vacuum).

### Running jobs on the JLab ifarm
The nodes used to submit jobs (currectly) should be ifarm1401 and ifarm1402. These are Centos7.2 nodes with relatively recent packages (G4, ROOT and so on). The Common Environment Version used at the time of writing is 2.0 (from Oct 2016).

The simulation package "should" compile and run fine on these nodes. 

#### Location
PREX code should reside in the parity work directory (/lustre/expphy/work/halla/parity). The output code should sit o nthe volatile disk (/lustre/expphy/volatile/halla/parity). You will need to be in the hall A and parity groups at JLab in order to have write access to these areas.

#### Job management
Jobs should be managed through the JLab swif system (https://scicomp.jlab.org/docs/swif). In order to help with job submission a script is provided in this package (scripts/jlabSubmit.py -- note that the job name cannot be longer than 150 characters so you should have relatively short names for your configurations). 

This script will create the folder structure, tarball and copy the necesary files that are needed for one simulation to be run. It also creates a .xml file with the information needed for swif to manage all the jobs. 

Before starting to submit jobs one needs to create a workflow for swif. For example:
```
swif create -workflow prexSim
```

Basic example after editing the jlabSubmit file to point to the correct locations and to use the correct configuration:
```
./jlabSubmit.py
cd jobs
swif add-jsub -workflow prexSim -script ConfigName.xml
swif run -workflow prexSim
swif status -workflow prexSim
```
Jobs will get submitted to the farm and will report back with error messages. Swif has the capability to resubmit jobs in bulk once you addressed the issue (sometimes happens that the system had a problem and you can just retry).

After jobs are completed analysis can proceed as normal.

## Package contents
This folder contains:

* **prex_sim.cc:**	Main Routine for Simulation.
* **CMakeLists.txt**	cmake contruct for Makefile
* **vis.mac:**		Can be called from the simulation prompt in order to run visualization. WARNING: Analysis and visualization cannot run concurrently.
* **include/:**		Header files for Simulation Classes.
* **src/:**		Source files for Simulation Classes.
* **scripts/:**		Folder with scripts to submit large number of jobs to farms.
* **rootScripts/:**	Folder for small analysis scripts that can be run from the root prompt.

##Output tree

The data structure of the NTuple is as follows:

 `ion:x:y:z:x0:y0:z0:kineE:px:py:pz:kineE0:px0:py0:pz0:kineE1:px1:py1:pz1:kineE2:px2:py2:pz2:type:volume:theta0:theta1:theta2:ev_num:process:event:creator:hit:kineE_org:theta_org:track:diffXS:totXS:rate`

During an event, very time that a particle hits a Sensitive 
Detector, it creates a Hit with the following data:

* **ion**	-if using Geant, ion is 1 if the particle was created by the process of "eIoni", which is how Geant defines Moller Scattering. Using this as a cut will give you only the Moller2 particles, as defined above.
* **x**	-x-position where the particle hit the Sensitive Detector
* **y**	-y-position where the particle hit the Sensitive Detector
* **z**	-z-position where the particle hit the Sensitive Detector
* **x0**	-x-position where the Moller interaction occurred
* **y0**	-y-position where the Moller interaction occurred
* **z0**	-z-position where the Moller interaction occurred
* **kineE**	-Energy of the particle when it hit the Sensitive Detector
* **px**	-Unit vector of momentum in x-direction at the Sensitive Detector
* **py**	-Unit vector of momentum in y-direction at the Sensitive Detector
* **pz**	-Unit vector of momentum in z-direction at the Sensitive Detector
* **kineE0**	-Kinetic Energy of the original electron which interacted inside the target and created particle Moller1 and Moller2 at the interaction point (x0,y0,z0) 
* **px0**	-Unit vector of momentum in x-direction of Moller0 right before interacting
* **py0**	-Unit vector of momentum in y-direction of Moller0 right before interacting
* **pz0**	-Unit vector of momentum in z-direction of Moller0 right before interacting
* **kineE1**	-Kinetic Energy of the original electron which interacted inside the target and created particle Moller1 and Moller2 at the interaction point (x1,y1,z1) 
* **px1**	-Unit vector of momentum in x-direction of Moller1 right after interacting
* **py1**	-Unit vector of momentum in y-direction of Moller1 right after interacting
* **pz1**	-Unit vector of momentum in z-direction of Moller1 right after interacting
* **kineE2**	-Kinetic Energy of the original electron which interacted inside the target and created particle Moller1 and Moller2 at the interaction point (x2,y2,z2)
* **px2**	-Unit vector of momentum in x-direction of Moller2 right after interacting.
* **py2**	-Unit vector of momentum in y-direction of Moller2 right after interacting.
* **pz2**	-Unit vector of momentum in z-direction of Moller2 right after interacting.
* **type**	-Particle type( electron=0, positron=1, proton=2, antiproton=3, gamma=4, other=5 )
* **volume**	-Volume number of the Sensitive Detector that recorded this Hit.
* **theta0**	-Angle with respect to the beam axis of Moller0 right before interacting.
* **theta1**	-Angle with respect to the beam axis of Moller1 right after interacting.
* **theta2**	-Angle with respect to the beam axis of Moller2 right after interacting.
* **ev_num**	-Event number, as assigned by Geant. An event is defined as the set of particles and interactions that happen after the Particle Genarator 'throws' a particle into the spectrometer.
* **process**	-Not properly defined at the moment
* **event**	-Not properly defined at the moment.
* **creator**	-Process that created this particle, as assigned by G4
	* eIoni->         0     
	* conv->          1
	* Decay->         2
	* annihil->       3
	* Brem->          4
	* phot->          5
	* compt->         6
	* Transportation->7
	* msc->           8
	* All Others->    9
* **hit**	-Same as trackID (see below).
* **kineE_org** - Vertex energy of THIS particle. 
* **theta_org** - Scattering Angle of THIS particle.
* **track**	-Track number as assigned by GEANT. Each particle created in an Event is given an incremental trackID number. In this case, Moller1 has track=1 and Moller2 has track=2.
* **diffXS**	-Differential cross-section calculated in generator.
* **totXS**	-Total cross-section calculated in generator.
* **rate**    -Rate calculated in generator.
