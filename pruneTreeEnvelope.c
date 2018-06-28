#include <sstream>
#include <string>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TApplication.h"
#include <vector> 

//From remolltypes.hh
struct remollGenericDetectorHit_t {
  int det;
  int id;
  int trid ;
  int pid;
    int gen;
    int mtrid;
    double t;
    double x, y, z;
    double xl, yl, zl;
    double r, ph;
    double px, py, pz;
    double pxl, pyl, pzl;
    double sx, sy, sz;
    double p, e, m;
    double vx, vy, vz;
    
    remollGenericDetectorHit_t() : det(0), id(0), trid(0), pid(0), gen(0), mtrid(0),
    t(0), x(0),y(0),z(0),xl(0),yl(0),zl(0),r(0),ph(0),px(0),py(0),pz(0),pxl(0),pyl(0),
    pzl(0),sx(0),sy(0),sz(0),p(0),e(0),m(0),vx(0),vy(0),vz(0)
    {}
};

struct remollEventParticle_t {
  int pid;
    double vx, vy, vz;
    double px, py, pz;
    double sx, sy, sz;
    double th, ph, p;
    double tpx, tpy, tpz;
  std::vector<double> tjx, tjy, tjz; //Trajectory information
    remollEventParticle_t() : vx(0), vy(0), vz(0),px(0),py(0),pz(0),
    sx(0),sy(0),sz(0),th(0),ph(0),p(0),tpx(0),tpy(0),tpz(0)
    {}
};

#ifdef __MAKECINT__ 
#pragma link C++ class vector< remollGenericDetectorHit_t >+; 
#pragma link C++ class vector< remollEventParticle_t >+; 
#endif
#define pi 3.141592653589793238462643383279502884L
remollGenericDetectorHit_t trim(remollGenericDetectorHit_t hit)
{
    remollGenericDetectorHit_t newHit;
    newHit.det = hit.det;
    newHit.id = hit.id;
    newHit.pid = hit.pid;
    newHit.trid = hit.trid;
    return newHit; 
}

remollEventParticle_t trim(remollEventParticle_t part)
{
    remollEventParticle_t newPart;
    newPart.pid = part.pid;
    newPart.tjx = part.tjx;
    newPart.tjy = part.tjy;
    newPart.tjz = part.tjz;
    return newPart;
}


const double septantVal = 2*pi / 14.0; 

double getAngle(remollGenericDetectorHit_t hit)
{
    return atan2(hit.y, -1 * hit.x);
}
double getAngle(double x, double y)
{
    return atan2(y, -1 * x);
}

remollEventParticle_t rotateVector(remollEventParticle_t part)
{
    //std::cout << "Rotating " << std::endl;
    remollEventParticle_t newPart;
    newPart.pid = part.pid;
    const double s = sin(2 * pi / 7.0);
    const double c = cos(2 * pi / 7.0);

    double x, y;
    for (int i = 0; i < part.tjx.size(); i++)
    {
        x = part.tjx.at(i);   
        y = part.tjy.at(i);   
        while (abs(getAngle(x, y) >= septantVal))
        {
            double tX = x * c - y * s;
            double tY = x * s + y * c;
            x = tX;
            y = tY;
        }
        newPart.tjx.push_back(x);
        newPart.tjy.push_back(y);
        newPart.tjz.push_back(part.tjz.at(i));
    }
    //std::cout << "Rotated " << std::endl;
    return newPart;
}
remollGenericDetectorHit_t rotateVector(remollGenericDetectorHit_t hit)
{
    const double s = sin(2 * pi / 7.0);
    const double c = cos(2 * pi / 7.0);
    remollGenericDetectorHit_t newHit;// = new remollGenericDetectorHit_t();
    newHit.z = hit.z;
    newHit.pz = hit.pz;
    newHit.id = hit.id;
    newHit.det = hit.det;
    newHit.pid = hit.pid;

    newHit.x = hit.x * c - hit.y * s;
    newHit.px = hit.px * c - hit.py * s;

    newHit.y = hit.x * s + hit.y * c;
    newHit.py = hit.px * s + hit.py * c;

    return newHit;
}
void pruneTreeEnvelope(std::string file="tracking.root", int detid=28, bool forceSeptant=true)
{
    std::vector < remollGenericDetectorHit_t > *fHit = 0;
    std::vector < remollEventParticle_t > *fPart = 0;
    std::ostringstream os;
    os << "tracking_optimized_det" << detid << ".root";
    std::string fileName = os.str();
    TFile *old = new TFile(file.c_str());
    TTree *oldTree = (TTree*)old->Get("T");
    TFile *newFile = new TFile(fileName.c_str(),"RECREATE", "", 1);

    TTree* newTree = new TTree("T", "Optimized Tree of Tracks");
    oldTree->SetBranchAddress("hit", &fHit); 
    oldTree->SetBranchAddress("part", &fPart); 
    std::vector < remollGenericDetectorHit_t > *hitCopy = new std::vector < remollGenericDetectorHit_t > ;
    std::vector < remollEventParticle_t > *partCopy = new std::vector < remollEventParticle_t > ;

    newTree->Branch("hit", &hitCopy);
    newTree->Branch("part", &partCopy);
    //newTree->AutoSave();
    //oldTree->Print();
    for (size_t j = 0; j < oldTree->GetEntries(); j++)
    {
        if (j%10000 == 0) 
        {
            std::cerr << "\r" <<  j << "/" << oldTree->GetEntries() << " - " << (j*1.0)/oldTree->GetEntries() * 100 << "%";
        }
     
        oldTree->GetEntry(j);
        //std::cout << "Hits: " << fHit->size() << std::endl;
        //std::cout << "Parts: " << fPart->size() << std::endl;

        std::vector<int> goodTRID;  
        std::vector<int> worthyTRID;

        for (size_t i = 0; i < fHit->size(); i++)
        {
            remollGenericDetectorHit_t hit = fHit->at(i); 
            //Get all track ids that hit into desired det
            if (hit.det == detid)
            {
                goodTRID.push_back(hit.trid);
            }
        }
        
        for (size_t i = 0; i < fPart->size();i++)
        {
            remollEventParticle_t part = fPart->at(i);
            for (size_t k = 0; k < goodTRID.size(); k++)
            {
                //Of track ids that hit into desired det, get those that are saved
                if (i == goodTRID.at(k))
                {
                    worthyTRID.push_back(i);
                    if (forceSeptant) part = rotateVector(part);
                    partCopy->push_back(trim(part));
                    break;
                }
            }   
        }
        for (size_t k = 0; k < worthyTRID.size(); k++)
        {
            int trid = worthyTRID.at(k);
            for (size_t i = 0; i < fHit->size(); i++)
            {
                remollGenericDetectorHit_t hit = fHit->at(i); 
                //and save the corresponding hit aswell
                if (trid == hit.trid)
                {
                    while (forceSeptant && abs(getAngle(hit)) >= septantVal)
                    {
                        hit = rotateVector(hit);
                    }
                    hitCopy->push_back(trim(hit));
                    break;
                }

            }
        }
        if (hitCopy->size() > 0)
            newTree->Fill();
        hitCopy->clear();
        partCopy->clear();
    }
    newTree->Write("", TObject::kOverwrite);
    newTree->Print();
    old->Close();
    newFile->Close();
}

