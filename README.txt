To use the make commands to work with remoll v2.0.0 output,
you need to copy a libremoll.so dictionary from the same build of remoll
the output file is from. libremoll.so must be in the directory after -L in the
makefile (default is .)

pruneTree.c takes a root file (or list of files) and cuts out most summary data and
all events and hits that miss the user selected detector. Septant folding also available

pruneTreeEnvelope.c optimizes a root tree for enveloping. It rotates and mirrors 
entires tracks in to the positive half of the 0th septant. It also cuts out all
tracks that don't hit the desired detector and aren't the desired PID (using a
mark and sweep type algorithm)

If you get a segfault check to see if remoll was run with tracking set properly

old_rad_dose.cc is Rakitha's old analysis script and the new one is the one I'm working on updating.

I have updated the energy ranges to look at 0-10, 10-30, and 30-infinity energies of particles.

The rad_dose_cip.cc reads the output of prexSim and only looks at the 30-infinity energy range. It makes fewer plots, as it is just used to make a benchmark analysis of the prexSim output. 

It would be nice to simulate prex in remoll, but getting the entire geometry over, instead of just my rebuilt dump geometry file, would be too difficult, so it is only to be used as a dump monitoring simulation for now (SAMs rad sims are done in prexSim using ciprian's analysis code on the ifarm).
