old_rad_dose.cc is Rakitha's old analysis script and the new one is the one I'm working on updating.

I have updated the energy ranges to look at 0-10, 10-30, and 30-infinity energies of particles.

The rad_dose_cip.cc reads the output of prexSim and only looks at the 30-infinity energy range. It makes fewer plots, as it is just used to make a benchmark analysis of the prexSim output. 

It would be nice to simulate prex in remoll, but getting the entire geometry over, instead of just my rebuilt dump geometry file, would be too difficult, so it is only to be used as a dump monitoring simulation for now (SAMs rad sims are done in prexSim using ciprian's analysis code on the ifarm).
