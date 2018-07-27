#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

#include "TTree.h"
#include "TFile.h"
#include "remolltypes.hh"
#include <vector> 

  
# define pi 3.141592653589793238462643383279502884L
remollGenericDetectorHit_t trim(remollGenericDetectorHit_t hit)
{
    remollGenericDetectorHit_t newHit;
    newHit.det = hit.det;
    newHit.id = hit.id;
    newHit.trid=0;
    newHit.pid = hit.pid;
    newHit.gen=0;
    newHit.mtrid=0;
    newHit.x = hit.x;
    newHit.y = hit.y;
    newHit.z = hit.z;
    newHit.xl=0;
    newHit.yl=0;
    newHit.zl=0;
    newHit.r=0;
    newHit.ph=0;
    newHit.px = hit.px;
    newHit.py = hit.py;
    newHit.pz = hit.pz;
    newHit.pxl=0;
    newHit.pyl=0;
    newHit.pzl=0;
    newHit.sx=0;
    newHit.sy=0;
    newHit.sz=0;
    newHit.p=0;
    newHit.e=0;
    newHit.m=0;
    newHit.vx=0;
    newHit.vy=0;
    newHit.vz=0;
    return newHit; 
}

void trim(remollEvent_t* newEv, remollEvent_t *ev)
{
    //For externalGenerator only xs, Q2, W2, A
    newEv->A = ev->A;
    newEv->Am = 0;//ev->Am;
    newEv->xs = ev->xs;
    newEv->Q2 = ev->Q2;
    newEv->W2 = ev->W2;
    newEv->thcom = 0;//ev->thcom;
    newEv->beamp = 0;//ev->beamp;
}

const double septantVal = 2*pi / 14.0; 

double getAngle(remollGenericDetectorHit_t hit)
{
    return atan2(hit.y, -1*hit.x);
}

remollGenericDetectorHit_t rotateVector(remollGenericDetectorHit_t hit)
{
    remollGenericDetectorHit_t newHit = trim(hit);

    double s = sin(2 * pi / 7.0);
    double c = cos(2 * pi / 7.0);

    newHit.x = hit.x * c - hit.y * s;
    newHit.px = hit.px * c - hit.py * s;

    newHit.y = hit.x * s + hit.y * c;
    newHit.py = hit.px * s + hit.py * c;

    return newHit;
}

void pruneTree(std::string file="remollin.root", int detid=28, bool forceSeptant=true)
{   
    int len = file.size();
    int dotPos = file.rfind(".");   
    std::vector< std::string > fileList;
    if (file.substr(dotPos) != ".root")
    {
        ifstream inFile(file.c_str()); 
        std::string temp;
        while (inFile >> temp)
        {
            std::cout << "Found: " << temp << std::endl;
            fileList.push_back(temp);
        }
    }
    else
    {
        fileList.push_back(file);
    }

    std::vector < remollGenericDetectorHit_t > *fHit = 0;
    remollEvent_t *fEv = 0;
    std::ostringstream os;
    os << file.substr(0, dotPos) << "_optimized_det" << detid << ".root";
    std::string fileName = os.str();
    
    TFile *newFile = new TFile(fileName.c_str(),"RECREATE", "", 1);
    std::vector < remollGenericDetectorHit_t > *hitCopy = new std::vector < remollGenericDetectorHit_t > ;
    remollEvent_t *eventCopy = new remollEvent_t();
    TTree* newTree = new TTree("T", "Reduced Tree");
    
    newTree->Branch("hit", &hitCopy);
    newTree->Branch("ev", &eventCopy);
    
    for (size_t i = 0; i < fileList.size(); i++)
    {
        TFile *old = new TFile(fileList.at(i).c_str());
        TTree *oldTree = (TTree*)old->Get("T");
    
        oldTree->SetBranchAddress("hit", &fHit); 
        oldTree->SetBranchAddress("ev", &fEv); 
         
        for (size_t j = 0; j < oldTree->GetEntries(); j++)
        {
            if (j%10000 == 0) 
            {
                std::cerr << "\r" << "File " << i << "/" << fileList.size() << ", Entry " <<  j << "/" << oldTree->GetEntries() << " - " << (j*1.0)/oldTree->GetEntries() * 100 << "%";
            }
            oldTree->GetEntry(j);
            trim(eventCopy, fEv);
            for (size_t i = 0; i < fHit->size(); i++)
            {
                remollGenericDetectorHit_t hit = fHit->at(i); 
                if (hit.det == detid)
                {
                    while (forceSeptant && TMath::Abs(getAngle(hit)) >= septantVal)
                    {
                        //std::cout << "Rotating..." << std::endl;   
                        hit = rotateVector(hit);
                        //std::cout << "\tto seventh #" << getAngle(hit) * -7/(2.0 * pi)<< std::endl;   
                    }
                    //std::cout << "Done!" << std::endl;
                    hitCopy->push_back(trim(hit));
                }
            }

            if (hitCopy->size() > 0)
                newTree->Fill();
            hitCopy->clear();
        }
        oldTree->ResetBranchAddresses();
        old->Close();
        delete old;
    }
    newFile = newTree->GetCurrentFile();
    //newTree->Write("", TObject::kOverwrite);
    newFile->Write();
    newFile->Close();
}

int main(int argc, char **argv)
{
    std::string fileString = "remollin.root";
    int detid = 28;
    bool forceSeptant = true;
    if (argc <= 1 || argc > 4)
    {
        std::cerr << "Usage: ./pruneTree char*:filename int:detid y/n:rotateIntoSeptant" << std::endl;
        exit(0);
    }
    if (argc >= 2) 
    {
        std::string fileName(argv[1]); 
        fileString = fileName;
    }
    if (argc >= 3)
    {
        detid = atoi(argv[2]);    
    }
    if (argc >= 4)
    {
        forceSeptant = (argv[3][0] == 'y');
    }
    pruneTree(fileString, detid, forceSeptant);
}

