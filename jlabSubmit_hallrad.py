#!/usr/bin/python
from subprocess import call
import sys, os, time, tarfile

def main():

#FIXME Update these
    email = "cameronc@jlab.org"

    stage = raw_input("Please enter the indicative name (merged, block1_height_10cm, etc.): ")
    number = raw_input("Please enter the number (100M, etc.): ")
    identifier = "moller_"+stage+"_"+number#raw_input("Please enter the identifier: ")

    #sourceDir = "/work/halla/parity/disk1/ciprian/prexSim"
    sourceDir = "/work/halla/parity/disk1/moller12gev/cameronc/remoll"
    sourceMasterDir = "/work/halla/parity/disk1/moller12gev/cameronc/remoll"
    outputDir = "/lustre/expphy/volatile/halla/parity/cameronc/remoll/output/"+stage+"_hallRad"
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)

    jobName=identifier

    if not os.path.exists(outputDir+"/"+jobName+"/log"):
        os.makedirs(outputDir+"/"+jobName+"/log")
    createXMLfile(sourceDir,outputDir,jobName,identifier,stage,email)

    call(["cp",sourceDir+"/rad_analysis/libremoll.so",
              outputDir+"/"+jobName+"/libremoll.so"])
    call(["cp",sourceMasterDir+"/rad_analysis/hallRad",
              outputDir+"/"+jobName+"/hallRad"])
    print "All done for configuration ",identifier

def createXMLfile(source,writeDir,idRoot,name,stag,email):

    if not os.path.exists(source+"/output/jobs"):
        os.makedirs(source+"/output/jobs")

    f=open(source+"/output/jobs/"+idRoot+"_hallRad.xml","w")
    f.write("<Request>\n")
    f.write("  <Email email=\""+email+"\" request=\"false\" job=\"true\"/>\n")
    f.write("  <Project name=\"moller12gev\"/>\n")

    f.write("  <Track name=\"analysis\"/>\n")
#    f.write("  <Track name=\"analysis\"/>\n")

    f.write("  <Name name=\""+name+"_hallRad\"/>\n")
    f.write("  <OS name=\"centos7\"/>\n")
    f.write("  <Memory space=\"3500\" unit=\"MB\"/>\n")

    f.write("  <Command><![CDATA[\n")
    f.write("    pwd\n")
    f.write("    ls ../../"+idRoot+"/*"+idRoot+"*/remollout.root >> list_"+idRoot+".txt\n")
    f.write("    ./hallRad --infile list_"+idRoot+".txt --detList 101\n")
    
    f.write("  ]]></Command>\n")

    idName= writeDir+"/"+idRoot
    f.write("  <Job>\n")
    f.write("    <Input src=\""+idName+"/hallRad\" dest=\"hallRad\"/>\n")
    f.write("    <Input src=\""+idName+"/libremoll.so\" dest=\"libremoll.so\"/>\n")
    f.write("    <Output src=\"list_"+idRoot+".txt\" dest=\""+idName+"/list_"+idRoot+".txt\"/>\n")
    f.write("    <Output src=\"list_"+idRoot+"_hallRad.root\" dest=\""+idName+"/list_"+idRoot+"_hallRad.root\"/>\n")

    f.write("    <Stdout dest=\""+idName+"/log/log.out\"/>\n")
    f.write("    <Stderr dest=\""+idName+"/log/log.err\"/>\n")
    f.write("  </Job>\n\n")

    f.write("</Request>\n")
    f.close()
    return 0

if __name__ == '__main__':
    main()
