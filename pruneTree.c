#include <sstream>
#include <string>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TApplication.h"
#include <vector> 
struct remollGenericDetectorHit_t {
  int det;
  int id;
  //int trid;
  int pid;
  //int gen;
  //int mtrid;
  //double t;
  double x, y, z;
  //double xl, yl, zl;
  //double r, ph;
  double px, py, pz;
  //double pxl, pyl, pzl;
  //double sx, sy, sz;
  //double p, e, m;
  //double vx, vy, vz;
};
  
#ifdef __MAKECINT__ 
#pragma link C++ class vector< remollGenericDetectorHit_t >+; 
#endif
# define pi 3.141592653589793238462643383279502884L
double getAngle(remollGenericDetectorHit_t hit)
{
    return atan2(hit.y, -1 * hit.x);
}

remollGenericDetectorHit_t rotateVector(remollGenericDetectorHit_t hit)
{
    remollGenericDetectorHit_t newHit;// = new remollGenericDetectorHit_t();
    newHit.z = hit.z;
    newHit.pz = hit.pz;
    newHit.id = hit.id;
    newHit.det = hit.det;
    newHit.pid = hit.pid;

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
    std::vector < remollGenericDetectorHit_t > *fHit = 0;
    std::ostringstream os;
    os << "remollin_optimized_det" << detid << ".root";
    std::string fileName = os.str();
    TFile *old = new TFile(file.c_str());
    TTree *oldTree = (TTree*)old->Get("T");
    TFile *newFile = new TFile(fileName.c_str(),"RECREATE", "", 0);
    const int numBranches = 5;
    const char* const branchNames[] ={
        //"hit", "hit.det", "hit.pid", 
        //"hit.x", "hit.y", "hit.z", 
        //"hit.px", "hit.py", "hit.pz", 
        "ev", "xs", "Q2", "W2", "A" };

    oldTree->SetBranchStatus("*", 0);
    oldTree->SetBranchStatus("*.*", 0);
    for (int i = 0; i < numBranches; i++)
    {
        oldTree->SetBranchStatus(branchNames[i],1);
    }
    
    TTree* newTree = (TTree*)oldTree->CloneTree(0);
    //TTree* newTree = new TTree();
    oldTree->SetBranchStatus("*", 1);
    oldTree->SetBranchStatus("*.*", 1);
    oldTree->SetBranchAddress("hit", &fHit); 
    std::vector < remollGenericDetectorHit_t > *hitCopy = new std::vector < remollGenericDetectorHit_t > ;
   
    //std::cout << hitCopy->size() << std::endl;
    newTree->Branch("hit", &hitCopy, 32000, 0);
    //newTree->AutoSave();
    for (size_t j = 0; j < oldTree->GetEntries(); j++)
    {
        if (j%10000 == 0) 
        {
            std::cerr << "\r" <<  j << "/" << oldTree->GetEntries() << " - " << (j*1.0)/oldTree->GetEntries() * 100 << "%";
        }
        oldTree->GetEntry(j);
        for (size_t i = 0; i < fHit->size(); i++)
        {
            //std::cout << "\t" << i << "/" << fHit->size() << std::endl;
            remollGenericDetectorHit_t hit = fHit->at(i); 
            //std::cout << "\t\thc" << hitCopy->size() << std::endl; //all the cout keeps it from breaking :)
            if (hit.det == detid)
            {
                //std::cout << "good hit" << std::endl;
                while (forceSeptant && abs(getAngle(hit)) >= (2*pi/14.0))
                {
                    //std::cout << "Rotating..." << std::endl;   
                    hit = rotateVector(hit);
                    //std::cout << "\tto seventh #" << getAngle(hit) * -7/(2.0 * pi)<< std::endl;   
                }
                //std::cout << "Done!" << std::endl;   
                hitCopy->push_back(hit);
            }
        }

        if (hitCopy->size() > 0)
            newTree->Fill();
        //std::cout << "entries" << newTree->GetEntries() << std::endl;
        hitCopy->clear();
    }
    newTree->Write("", TObject::kOverwrite);
    newTree->Print();
    old->Close();
    newFile->Close();
}

