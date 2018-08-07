#!/usr/bin/python
from subprocess import call
import sys, os, time, tarfile

def main():

#FIXME Update these
    email = "chandan@jlab.org"

    configuration = "moller"

    identifier = raw_input("Please enter the identifier: ")

    # To print an xml file that allows for parameter space searches do this here
    #f = open('../geometry/'+identifier+'.xml', 'w')
    #fileout = '    <constant name="full_sam_r_outward_offset" value="' + offset + '.0/10"/>\n    <constant name="sam_mid_dist" value="full_sam_r_outward_offset + sam_bot_face_sep + sam_can_length/2."/>\n    <constant name="sam_quartz_mid_dist" value="full_sam_r_outward_offset + sam_quartz_bot_face + sam_quartz_length/2."/>\n    <constant name="sam_quartz_height" value="' + thickness + '.0/10"/>'
    #f.write(fileout)
    #f.close()

    #sourceDir = "/work/halla/parity/disk1/ciprian/prexSim"
    sourceDir = "/work/halla/parity/disk1/moller12gev/cameronc/remoll/"
    outputDir = "/lustre/expphy/volatile/halla/parity/cameronc/remoll/output/"+identifier
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)
    nrEv   = 100000 #900000
    nrStart= 1
    nrStopActual = 100 #60
    nrStop = nrStopActual+1 #60
    ###format should be Name (removed _)
    #"SAMs_noAl" #6inDonut_SAMs"  (spherical, cylindrical, noFace, noAl, noQ, noQnoAl)
#</FIXME>

    print('Running ' + str(nrEv*(nrStop - nrStart)) + ' events...')

    jobName=configuration + '_' + identifier + '_%03dkEv'%(nrEv/1000)

    ###tar exec+geometry
    make_tarfile(sourceDir,identifier)

    for nr in range(nrStart,nrStop): # repeat for nr jobs
        print("Starting job setup for jobID: " + str(nr))

        jobFullName = jobName + '_%05d'%nr
        createMacFiles(outputDir+"/"+jobFullName, sourceDir, nrEv, nr, identifier)

        ###copy tarfile
        call(["cp",sourceDir+"/rad_analysis/z_config.tar.gz",
              outputDir+"/"+jobFullName+"/z_config.tar.gz"])

    createXMLfile(sourceDir,outputDir,jobName,identifier,nrStart,nrStop,email)

    print "All done for configuration ",configuration,"_",identifier," for #s from ",nrStart, " to ", nrStopActual


def createMacFiles(outDir,sourceDir,nrEv,jobNr,identifier):

    if not os.path.exists(outDir+"/log"):
        os.makedirs(outDir+"/log")

    f=open(outDir+"/"+"runexample_"+identifier+".mac",'w')
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
    f.write("/remoll/filename remoll_"+identifier+".root\n")
    f.write("/run/beamOn "+str(nrEv)+"\n")
    f.close()
    #seedA=long(time.time()+jobNr)
    #seedB=long(time.time()*100+jobNr)
    #f.write("/random/setSeeds "+str(seedA)+" "+str(seedB)+"\n")
    return 0

def createXMLfile(source,writeDir,idRoot,modification,nStart,nStop,email):

    if not os.path.exists(source+"/rad_analysis/jobs"):
        os.makedirs(source+"/rad_analysis/jobs")

    f=open(source+"/rad_analysis/jobs/"+idRoot+".xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"moller12gev\"/>\n")

#    f.write("  <Track name=\"debug\"/>\n")
#    f.write("  <Track name=\"analysis\"/>\n")
    f.write("  <Track name=\"simulation\"/>\n")

    f.write("  <Name name=\""+idRoot+"\"/>\n")
    f.write("  <OS name=\"centos7\"/>\n")
    f.write("  <Memory space=\"3500\" unit=\"MB\"/>\n")

    f.write("  <Command><![CDATA[\n")
    f.write("    pwd\n")
    f.write("    tar -zxvf z_config.tar.gz\n")
    f.write("    ./remoll runexample_"+modification+".mac\n")
    f.write("  ]]></Command>\n")

    for nr in range(nStart,nStop): # repeat for nr jobs
        idName= writeDir+"/"+idRoot+'_%05d'%(nr)
        f.write("  <Job>\n")
        f.write("    <Input src=\""+idName+"/runexample_"+modification+".mac\" dest=\"runexample_"+modification+".mac\"/>\n")
        f.write("    <Input src=\""+idName+"/z_config.tar.gz\" dest=\"z_config.tar.gz\"/>\n")

        f.write("    <Output src=\"remoll_"+modification+".root\" dest=\""+idName+"/remoll_"+modification+".root\"/>\n")
        f.write("    <Stdout dest=\""+idName+"/log/log.out\"/>\n")
        f.write("    <Stderr dest=\""+idName+"/log/log.err\"/>\n")
        f.write("  </Job>\n\n")

    f.write("</Request>\n")
    f.close()
    return 0

def make_tarfile(sourceDir,ident):
    print "making geometry tarball"
    if os.path.isfile(sourceDir+"/rad_analysis/z_config.tar.gz"):
        os.remove(sourceDir+"/rad_analysis/z_config.tar.gz")
    tar = tarfile.open(sourceDir+"/rad_analysis/z_config.tar.gz","w:gz")
    tar.add(sourceDir+"/remoll/build/remoll",arcname="remoll")
    #tar.add(sourceDir+"/macros/runexample_"+ident+".mac",arcname="runexample_"+ident+".mac") 
    #runexample overwrite could crash
    tar.add(sourceDir+"/remoll/map_directory",arcname="map_directory")
    tar.add(sourceDir+"/remoll/geometry/schema",arcname="geometry/schema")
    tar.add(sourceDir+"/remoll/geometry/materials.xml",arcname="geometry/materials.xml")
    tar.add(sourceDir+"/remoll/geometry/mollerParallel.gdml" ,arcname="geometry/mollerParallel.gdml") 
    tar.add(sourceDir+"/remoll/geometry/mollerMother_"+ident+".gdml" ,arcname="geometry/mollerMother_"+ident+".gdml") 
    tar.add(sourceDir+"/remoll/geometry/targetDaughter_"+ident+".gdml" ,arcname="geometry/targetDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/remoll/geometry/hallDaughter_"+ident+".gdml" ,arcname="geometry/hallDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/remoll/geometry/detectorDaughter_"+ident+".gdml" ,arcname="geometry/detectorDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/remoll/geometry/upstreamDaughter_"+ident+".gdml" ,arcname="geometry/upstreamDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/remoll/geometry/hybridDaughter_"+ident+".gdml" ,arcname="geometry/hybridDaughter_"+ident+".gdml")
    tar.add(sourceDir+"/remoll/geometry/dumpDaughter_"+ident+".gdml" ,arcname="geometry/dumpDaughter_"+ident+".gdml")
    #tar.add(sourceDir+"/remoll/geometry/"+ident+".xml",arcname="geometry/"+ident+".xml")

    tar.close()

if __name__ == '__main__':
    main()

