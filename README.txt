To use the make commands to compile rad_dose or hallRad to work with remoll v2.0.0 output you need to copy a libremoll.so dictionary from a recent remoll build directory. libremoll.so must be in one of the directories pointed to by LD_LIBRARY_PATH.
--- Use this command
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
---
or use in tcsh
setenv LD_PRELOAD ..../..path.../libremoll.so


old_rad_dose.cc is Rakitha's old analysis script and the new one is the one I'm working on updating.

I have updated the energy ranges to look at 0-10, 10-30, and 30-infinity energies of particles.

The rad_dose_cip.cc reads the output of prexSim and only looks at the 30-infinity energy range. It makes fewer plots, as it is just used to make a benchmark analysis of the prexSim output. 

It would be nice to simulate prex in remoll, but getting the entire geometry over, instead of just my rebuilt dump geometry file, would be too difficult, so it is only to be used as a dump monitoring simulation for now (SAMs rad sims are done in prexSim using ciprian's analysis code on the ifarm).
