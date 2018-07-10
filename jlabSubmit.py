#!/usr/bin/python
from subprocess import call
import sys, os, time, tarfile

def main():

#FIXME Update these
    email = "cameronc@jlab.org"

    configuration = "moller"
    #configuration = "prexI"
    #configuration = "crex5"
    #configuration = "prexII"
    #configuration = "moller"
    #configuration = "happex2"

    varied = "benchmark"
    #thickness = raw_input("Please enter the thickness in mm (integers up to 15): ")
    identifier = varied#raw_input("Please enter the identifier: ")

    #f = open('../geometry/'+identifier+'.xml', 'w')
    #fileout = '    <constant name="full_sam_r_outward_offset" value="' + offset + '.0/10"/>\n    <constant name="sam_mid_dist" value="full_sam_r_outward_offset + sam_bot_face_sep + sam_can_length/2."/>\n    <constant name="sam_quartz_mid_dist" value="full_sam_r_outward_offset + sam_quartz_bot_face + sam_quartz_length/2."/>\n    <constant name="sam_quartz_height" value="' + thickness + '.0/10"/>'
    #f.write(fileout)
    #f.close()

    #sourceDir = "/work/halla/parity/disk1/ciprian/prexSim"
    sourceDir = "/work/halla/parity/disk1/moller12gev/cameronc/remoll"
    outputDir = "/lustre/expphy/volatile/halla/parity/cameronc/remoll/output/"+varied
    nrEv   = 900000 #900000
    nrStart= 1
    nrStop = 11 #60
    ###format should be Name (removed _)
    #"SAMs_noAl" #6inDonut_SAMs"  (spherical, cylindrical, noFace, noAl, noQ, noQnoAl)
#</FIXME>

    print('Running ' + str(nrEv*(nrStop - nrStart)) + ' events...')

    jobName=configuration + '_' + identifier + '_%03dkEv'%(nrEv/1000)

    ###tar exec+geometry
    make_tarfile(sourceDir,configuration,identifier)

    for nr in range(nrStart,nrStop): # repeat for nr jobs
        print("Starting job setup for jobID: " + str(nr))

        jobFullName = jobName + '_%05d'%nr
        createMacFiles(configuration, outputDir+"/"+jobFullName, sourceDir, nrEv, nr, identifier)

        ###copy tarfile
        call(["cp",sourceDir+"/scripts/z_config.tar.gz",
              outputDir+"/"+jobFullName+"/z_config.tar.gz"])

    createXMLfile(sourceDir,outputDir,jobName,nrStart,nrStop,email)

    print "All done for configuration ",configuration,"_",identifier," for #s between ",nrStart, " and ", nrStop


def createMacFiles(config,outDir,sourceDir,nrEv,jobNr,identifier):

    if not os.path.exists(outDir+"/log"):
        os.makedirs(outDir+"/log")

    f=open(outDir+"/"+"/myRun.mac",'w')
    if identifier=="benchmark":
      f.write("/remoll/setgeofile geometry/mollerMother_merged.gdml\n")
    elif:
      f.write("/remoll/setgeofile geometry/mollerMother_"+config+".gdml\n")
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

def createXMLfile(source,writeDir,idRoot,nStart,nStop,email):

    if not os.path.exists(source+"/rad_analysis/jobs"):
        os.makedirs(source+"/rad_analysis/jobs")

    f=open(source+"/scripts/jobs/"+idRoot+".xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"moller12gev\"/>\n")

#    f.write("  <Track name=\"debug\"/>\n")
    f.write("  <Track name=\"simulation\"/>\n")

    f.write("  <Name name=\""+idRoot+"\"/>\n")
    f.write("  <OS name=\"centos7\"/>\n")
    f.write("  <Memory space=\"3500\" unit=\"MB\"/>\n")

    f.write("  <Command><![CDATA[\n")
    f.write("    pwd\n")
    f.write("    tar -zxvf z_config.tar.gz\n")
    f.write("    ./remoll myRun.mac\n")
    f.write("  ]]></Command>\n")

    for nr in range(nStart,nStop): # repeat for nr jobs
        idName= writeDir+"/"+idRoot+'_%05d'%(nr)
        f.write("  <Job>\n")
        f.write("    <Input src=\""+idName+"/myRun.mac\" dest=\"myRun.mac\"/>\n")
        f.write("    <Input src=\""+idName+"/z_config.tar.gz\" dest=\"z_config.tar.gz\"/>\n")

        f.write("    <Output src=\"remollout.root\" dest=\""+idName+"/remollout.root\"/>\n")
        f.write("    <Stdout dest=\""+idName+"/log/log.out\"/>\n")
        f.write("    <Stderr dest=\""+idName+"/log/log.err\"/>\n")
        f.write("  </Job>\n\n")

    f.write("</Request>\n")
    f.close()
    return 0

def make_tarfile(sourceDir,config,ident):
    print "making geometry tarball"
    if os.path.isfile(sourceDir+"/scripts/z_config.tar.gz"):
        os.remove(sourceDir+"/scripts/z_config.tar.gz")
    tar = tarfile.open(sourceDir+"/scripts/z_config.tar.gz","w:gz")
    tar.add(sourceDir+"/build/prexsim",arcname="prexsim")
    tar.add(sourceDir+"/geometry/schema",arcname="geometry/schema")
    tar.add(sourceDir+"/geometry/"+config+"_"+ident+".gdml" ,arcname="geometry/"+config+"_"+ident+".gdml")
    tar.add(sourceDir+"/geometry/kriptoniteDetectors.gdml",arcname="geometry/kriptoniteDetectors.gdml")
    tar.add(sourceDir+"/geometry/kriptoniteDetectors_withHRS.gdml",arcname="geometry/kriptoniteDetectors_withHRS.gdml")
    tar.add(sourceDir+"/geometry/subQ1HosesCylRedesign.gdml",arcname="geometry/subQ1HosesCylRedesign.gdml")
    tar.add(sourceDir+"/geometry/subTargetChamber.gdml",arcname="geometry/subTargetChamber.gdml")
    tar.add(sourceDir+"/geometry/subCollShields.gdml",arcname="geometry/subCollShields.gdml")
    tar.add(sourceDir+"/geometry/prex1Beampipe.gdml",arcname="geometry/prex1Beampipe.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe.gdml",arcname="geometry/subBeamPipe.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_fatP2end.gdml",arcname="geometry/subBeamPipe_fatP2end.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_4inDonut.gdml",arcname="geometry/subBeamPipe_4inDonut.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_noDonut.gdml",arcname="geometry/subBeamPipe_noDonut.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_steelTelePipe.gdml",arcname="geometry/subBeamPipe_steelTelePipe.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipeMoller.gdml",arcname="geometry/subBeamPipeMoller.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipeMoller_fatP2end.gdml",arcname="geometry/subBeamPipeMoller_fatP2end.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipeMoller_4inDonut.gdml",arcname="geometry/subBeamPipeMoller_4inDonut.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipeMoller_noDonut.gdml",arcname="geometry/subBeamPipeMoller_noDonut.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_MidVacuum.gdml",arcname="geometry/subBeamPipe_MidVacuum.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_12GeV_SAMs.gdml",arcname="geometry/subBeamPipe_12GeV_SAMs.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_6inDonut_SAMs.gdml",arcname="geometry/subBeamPipe_6inDonut_SAMs.gdml")
    tar.add(sourceDir+"/geometry/subBeamPipe_6inDonut_noSAMs.gdml",arcname="geometry/subBeamPipe_6inDonut_noSAMs.gdml")
    tar.add(sourceDir+"/geometry/subDumpShield.gdml",arcname="geometry/subDumpShield.gdml")
    tar.add(sourceDir+"/geometry/subSkyShineShield.gdml",arcname="geometry/subSkyShineShield.gdml")
    tar.add(sourceDir+"/geometry/subDumpShield_cover.gdml",arcname="geometry/subDumpShield_cover.gdml")
    tar.add(sourceDir+"/geometry/subDumpShield_2layer.gdml",arcname="geometry/subDumpShield_2layer.gdml")
    tar.add(sourceDir+"/geometry/materials.xml",arcname="geometry/materials.xml")
    tar.add(sourceDir+"/geometry/subHRSplatform.gdml",arcname="geometry/subHRSplatform.gdml")
    tar.add(sourceDir+"/geometry/subHRSplatform_withShield.gdml",arcname="geometry/subHRSplatform_withShield.gdml")
    tar.add(sourceDir+"/geometry/mollerDScollAndCoils.gdml",arcname="geometry/mollerDScollAndCoils.gdml")
    tar.add(sourceDir+"/geometry/mollerUScollAndCoils.gdml",arcname="geometry/mollerUScollAndCoils.gdml")
    tar.add(sourceDir+"/geometry/mollerDet.gdml",arcname="geometry/mollerDet.gdml")
    tar.add(sourceDir+"/geometry/"+ident+".gdml",arcname="geometry/"+ident+".gdml")
    tar.add(sourceDir+"/geometry/"+ident+".xml",arcname="geometry/"+ident+".xml")


    tar.close()

if __name__ == '__main__':
    main()

