#!/usr/bin/python
from subprocess import call
import sys, os, time, tarfile

def main():

#FIXME Update these
    email = "cameronc@jlab.org"

    config = "moller"

    identifier = raw_input("Please enter the identifier: ")
    #val = raw_input("Please enter the varied parameter in cm: ")

    ## To print an xml file that allows for parameter space searches do this here
   # f = open('../geometry/'+identifier+'.xml', 'w')
   # fileout = '    <constant name="USShield1_increase_height" value="'+val+'"/>\n    <consant name="USShield2_increase_height" value"'+val+'"/>\n'
   # f.write(fileout)
   # f.close()

    #sourceDir = "/work/halla/parity/disk1/ciprian/prexSim"
    sourceDir = "/work/halla/parity/disk1/moller12gev/cameronc/remoll"
    outDir = "/lustre/expphy/volatile/halla/parity/cameronc/remoll/output/"+identifier
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    nrEv   = 100000 #900000
    nrStart= 1
    nrStopActual = 1000 #60
    nrStop = nrStopActual+1 #60
    ###format should be Name (removed _)
    #"SAMs_noAl" #6inDonut_SAMs"  (spherical, cylindrical, noFace, noAl, noQ, noQnoAl)
#</FIXME>

    print('Running ' + str(nrEv*(nrStop - nrStart)) + ' events...')

    #jobName=config + '_' + identifier + '_' + val + "mm" + '_%03dkEv'%(nrEv/1000)
    jobName=config + '_' + identifier + '_%03dkEv'%(nrEv/1000)

    ###tar exec+geometry
    make_tarfile(sourceDir,config,identifier)

    for jobNr in range(nrStart,nrStop): # repeat for jobNr jobs
        print("Starting job setup for jobID: " + str(jobNr))

        jobFullName = jobName + '_%05d'%jobNr
        outDirFull=outDir+"/"+jobFullName
        createMacFiles(config, outDirFull, sourceDir, nrEv, jobNr, identifier)

        ###copy tarfile
        call(["cp",sourceDir+"/rad_analysis/z_config.tar.gz",
              outDir+"/"+jobFullName+"/z_config.tar.gz"])

    createXMLfile(sourceDir,outDir,jobName,nrStart,nrStop,email,identifier)

    print "All done for config ",config,"_",identifier," for #s between ",nrStart, " and ", nrStopActual


def createMacFiles(config,outDirFull,sourceDir,nrEv,jobNr,identifier):

    if not os.path.exists(outDirFull+"/log"):
        os.makedirs(outDirFull+"/log")

    f=open(outDirFull+"/"+"runexample_"+identifier+".mac",'w')
    f.write("/remoll/setgeofile geometry/mollerMother_"+identifier+".gdml\n")
    f.write("/run/initialize\n")
    f.write("/remoll/addfield map_directory/blockyHybrid_rm_3.0.txt\n")
    f.write("/remoll/addfield map_directory/blockyUpstream_rm_1.1.txt\n")
    f.write("/remoll/oldras true\n")
    f.write("/remoll/rasx 5 mm\n")
    f.write("/remoll/rasy 5 mm\n")
    f.write("/remoll/evgen/set beam\n")
    f.write("/remoll/beamene 11 GeV\n")
    f.write("/remoll/kryptonite/set false\n")
    f.write("/process/list\n")
    f.write("/remoll/filename remollout.root\n")
    f.write("/run/beamOn "+str(nrEv)+"\n")
    f.close()
    #seedA=long(time.time()+jobNr)
    #seedB=long(time.time()*100+jobNr)
    #f.write("/random/setSeeds "+str(seedA)+" "+str(seedB)+"\n")
    return 0

def createXMLfile(sourceDir,outDir,jobName,nrStart,nrStop,email,identifier):

    if not os.path.exists(sourceDir+"/rad_analysis/jobs"):
        os.makedirs(sourceDir+"/rad_analysis/jobs")

    f=open(sourceDir+"/rad_analysis/jobs/"+jobName+".xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"moller12gev\"/>\n")

#    f.write("  <Track name=\"debug\"/>\n")
#    f.write("  <Track name=\"analysis\"/>\n")
    f.write("  <Track name=\"simulation\"/>\n")

    f.write("  <Name name=\""+jobName+"\"/>\n")
    f.write("  <OS name=\"centos7\"/>\n")
    f.write("  <Memory space=\"3500\" unit=\"MB\"/>\n")

    f.write("  <Command><![CDATA[\n")
    f.write("    pwd\n")
    f.write("    tar -zxvf z_config.tar.gz\n")
    f.write("    ./remoll runexample_"+identifier+".mac\n")
    f.write("    ./pruneTree remollout.root 101 n\n")
    f.write("    rm -f remollout.root\n")
    f.write("  ]]></Command>\n")

    for number in range(nrStart,nrStop): # repeat for nr jobs
        idName= outDir+"/"+jobName+'_%05d'%(number)
        f.write("  <Job>\n")
        f.write("    <Input src=\""+idName+"/runexample_"+identifier+".mac\" dest=\"runexample_"+identifier+".mac\"/>\n")
        f.write("    <Input src=\""+idName+"/z_config.tar.gz\" dest=\"z_config.tar.gz\"/>\n")

        f.write("    <Output src=\"remollout_optimized_det101.root\" dest=\""+idName+"/remollout.root\"/>\n")
        f.write("    <Stdout dest=\""+idName+"/log/log.out\"/>\n")
        f.write("    <Stderr dest=\""+idName+"/log/log.err\"/>\n")
        f.write("  </Job>\n\n")

    f.write("</Request>\n")
    f.close()
    return 0

def make_tarfile(sourceDir,config,ident):
    print "making geometry tarball"
    if os.path.isfile(sourceDir+"/rad_analysis/z_config.tar.gz"):
        os.remove(sourceDir+"/rad_analysis/z_config.tar.gz")
    tar = tarfile.open(sourceDir+"/rad_analysis/z_config.tar.gz","w:gz")
    tar.add(sourceDir+"/build/remoll",arcname="remoll")
    tar.add(sourceDir+"/build/libremoll.so",arcname="libremoll.so")
    tar.add(sourceDir+"/rad_analysis/pruneTree",arcname="pruneTree")
    #tar.add(sourceDir+"/macros/runexample_"+ident+".mac",arcname="runexample_"+ident+".mac") 
    #runexample overwrite could crash
    tar.add(sourceDir+"/map_directory",arcname="map_directory")
    tar.add(sourceDir+"/geometry/schema",arcname="geometry/schema")
    tar.add(sourceDir+"/geometry/materials.xml",arcname="geometry/materials.xml")
    tar.add(sourceDir+"/geometry/mollerParallel.gdml" ,arcname="geometry/mollerParallel.gdml") 
    tar.add(sourceDir+"/geometry/mollerMother_"+ident+".gdml" ,arcname="geometry/mollerMother_"+ident+".gdml") 
    tar.add(sourceDir+"/geometry/targetDaughter_"+ident+".gdml" ,arcname="geometry/targetDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/hallDaughter_"+ident+".gdml" ,arcname="geometry/hallDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/detectorDaughter_"+ident+".gdml" ,arcname="geometry/detectorDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/upstreamDaughter_"+ident+".gdml" ,arcname="geometry/upstreamDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/hybridDaughter_"+ident+".gdml" ,arcname="geometry/hybridDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/dumpDaughter_"+ident+".gdml" ,arcname="geometry/dumpDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/"+ident+".xml",arcname="geometry/"+ident+".xml")

    tar.close()

if __name__ == '__main__':
    main()

