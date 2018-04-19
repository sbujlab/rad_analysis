/*
Rakitha Mon Aug 25 14:48:01 EDT 2014
Plot generated related to the radiation in the hall

Radiation plots
KE intercepted by the cylindrical and two disk detectors
vertex distribution on the cylindrical and two disk detectors

Vertex cuts are based on the shielding blocks

//////

Cameron Clarke Friday November 10 12:11:59 EDT 2017

*/

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <new>
#include <cstdlib>
#include <math.h>

#include <TRandom.h>
#include <TRandom3.h>
#include <TApplication.h>
#include <TSystem.h>

#include <TH2F.h>
#include <TH2D.h>
#include <TTree.h>
#include <TF1.h>
#include <TProfile.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TString.h> 
#include <TDatime.h>
#include <TStopwatch.h>
#include <stdexcept>
#include <time.h>
#include <cstdio>
#include <map>
#include <cassert>

#include <TMath.h>
#include <TStyle.h>
#include <TPaveStats.h>

#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TFrame.h>
#include <TObjArray.h>
#include <TVector2.h>
#include <TLatex.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#define __IO_MAXHIT 10000


//for generic hits
Double_t fEvRate;
Int_t fNGenDetHit;
Int_t fGenDetHit_det[__IO_MAXHIT];
Int_t fGenDetHit_trid[__IO_MAXHIT];
Int_t fGenDetHit_pid[__IO_MAXHIT];
Double_t fGenDetHit_P[__IO_MAXHIT];
Double_t fGenDetHit_PZ[__IO_MAXHIT];
Double_t fGenDetHit_X[__IO_MAXHIT];
Double_t fGenDetHit_Y[__IO_MAXHIT];
Double_t fGenDetHit_Z[__IO_MAXHIT];
Double_t fGenDetHit_VX[__IO_MAXHIT];
Double_t fGenDetHit_VY[__IO_MAXHIT];
Double_t fGenDetHit_VZ[__IO_MAXHIT];

// List of sensitive detectors:
const int n_detectors= 4;

Int_t SensVolume_v[n_detectors] = {100,6,9,53};//,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,51,52,53,54};
TString sdet[n_detectors]={"dumpDet","Roof","Dump","GateValve"};
// Idealized vacuum detector near dump = 100
// Hall roof = 6
// Hall walls = 7
// Hall floor = 8
// Dump walls = 9
// Dump lead shield left = 10
// Dump lead shield right = 11
// Neckdown: 12 - 21
// Nitrogen Atm wall = 22
// DS Dump = 23
// Dump water tank = 24
// Dump water = 25
// Dump extension beampipe = 51
// Telescoping beampipe = 52
// Gatevalve = 53
// Midpipe = 54

const int n_energy_ranges = 1;
const int n_particles = 3;

////Flux and power parameters Full range //indices [detector][pid] = [1][3] - e,gamma, n. 
Double_t flux_local[n_detectors][n_particles]={{0}};
Double_t power_local[n_detectors][n_particles]={{0}};

std::map<int,int> pidmap;
std::map<int,int> detectormap;
std::map<int,double> pidmass;

// FIXME get the hit_radius cuts right based on new beam pipes etc.
Double_t hit_radius_min[2] = {0.46038,0.46038}; //m inner radius of the beam pipe 45.72 cm and outer radius of the beam pipe 46.038 cm and radius of the detector plane is 1.9 m
Double_t hit_radius;
Double_t kineE;

//boolean switches

Bool_t earlybreak=kFALSE;              //exit early from the main tree entries loop

Bool_t kSaveRootFile=kTRUE;           //save histograms and canvases into a rootfile
Bool_t kShowGraphic=kTRUE;            //Show canvases and they will be saved the rootfile. Set this to false to show no graphics but save them to rootfile

//Boolean parameter to disable/enable saving histograms as png
Bool_t kVertices=kTRUE;               // Governs the cut region plotting
Bool_t kHits=kTRUE;               // Governs the sensitive shielding region hit position plotting

Bool_t kSave2DHisto=kVertices; // option to save 2D origin vertices for the different shielding block destination regions
Bool_t kShow2DPlots=kVertices;
Bool_t kSave2DVertexHisto=kVertices; // option to save 2D origin vertices for the different shielding block destination regions
Bool_t kShow2DVertexPlots=kVertices;
Bool_t kSave2DHitHisto=kHits; // option to save 2D hit position distributions for the different shielding block destination regions
Bool_t kShow2DHitPlots=kHits;


//end of boolean switches
void set_plot_style();

TFile * rootfile;
int main(Int_t argc,Char_t* argv[]) {
  TApplication theApp("App",&argc,argv);
  ofstream list_outputs;

  //remoll Tree
  TChain * Tmol =new TChain("T");
  //Cameron Clarke runs:
  //input info:
  int n_files;
  const int n_mills = 18;// FIXME number of million events
  Int_t n_events = n_mills*1e6;
  Int_t beamcurrent = 85;//uA
  TString added_file_array[n_mills]={""};//200]={""};
  if (n_mills==1){
    n_files = atoi(argv[2]) - 1;
    for (int v=0 ; v <= n_files ; v++){ 
      int r = v;
      ostringstream temp_str_stream2;
      ostringstream temp_str_stream3;
      temp_str_stream2<<r;
      TString vS;
      vS=temp_str_stream2.str();
      if (v==0){
        temp_str_stream3<<"/home/cameronc/gitdir/prex_cam/build/prex_dump_"<<argv[1]<<"_"<<n_mills<<"M/prex_dump_"<<argv[1]<<"_"<<n_mills<<"M.root";
      }
      else {
        temp_str_stream3<<"/home/cameronc/gitdir/prex_cam/build/prex_dump_"<<argv[1]<<"_"<<n_mills<<"M/prex_dump_"<<argv[1]<<"_"<<n_mills<<"M_"<<vS<<".root";
      }
      added_file_array[v]=temp_str_stream3.str();
      Tmol->Add(added_file_array[v]);
      std::cout<<"File: "<<added_file_array[v]<<std::endl;
    }
  }
  else if (n_mills==18){
    n_files = n_mills;//atoi(argv[2]) - 1;
    for (int v=0 ; v < n_files ; v++){ 
      int r = v;
      ostringstream temp_str_stream2;
      ostringstream temp_str_stream3;
      temp_str_stream2<<r;
      TString vS;
      vS=temp_str_stream2.str();
      temp_str_stream3<<"/home/cameronc/gitdir/prex_cam/build/out_"<<argv[1]<<"_"<<v+1<<"/prex_dump_"<<argv[1]<<"_"<<n_mills<<"M.root";
      added_file_array[v]=temp_str_stream3.str();
      Tmol->Add(added_file_array[v]);
      std::cout<<"File: "<<added_file_array[v]<<std::endl;
    }
  }
  
  ostringstream temp_str_stream4;
  temp_str_stream4<<"/home/cameronc/gitdir/prex_cam/rad_analysis/";//coll_prexII_"<<argv[1]<<"_"<<n_mills<<"M";//Name of folder for saving plots
  TString plotsFolder=temp_str_stream4.str();//Name of folder for saving plots
  ostringstream temp_str_stream5;
  temp_str_stream5<<plotsFolder<<"prex_dump_"<<argv[1]<<"_"<<n_mills<<"M_plots.root";//name of the rootfile to save generated histograms
  TString rootfilename=temp_str_stream5.str();//name of the rootfile to save generated histograms
  ostringstream temp_str_stream6;
  temp_str_stream6<<plotsFolder<<"list_outputs_prex_dump_"<<argv[1]<<"_"<<n_mills<<"M.txt";
  TString textfilename=temp_str_stream6.str();
  list_outputs.open(textfilename);
  list_outputs << "Contents of textout_flux and textout_power lists of strings" << std::endl;

  // Alright: Everything necessary exists for me to plot vertex vs. z plots, but the R vs. z plots are probably enough for now to be honest (need to be set to some absolute scale). It would be nice to have a radiation at top of hall plot vs. z (color mapped maybe) distribution as well though. I also want to generate plots of the last significant scattering vertex, not just the particle creation vertices, in case these are significantly different.

  //generic hit (for sens detectors)
  Tmol->SetBranchAddress("rate",&fEvRate);
  Tmol->SetBranchAddress("hit.n",&fNGenDetHit);
  Tmol->SetBranchAddress("hit.det",&fGenDetHit_det);
  Tmol->SetBranchAddress("hit.pid",&fGenDetHit_pid);
  Tmol->SetBranchAddress("hit.trid",&fGenDetHit_trid);
  Tmol->SetBranchAddress("hit.p",&fGenDetHit_P);
  Tmol->SetBranchAddress("hit.pz",&fGenDetHit_PZ);
  Tmol->SetBranchAddress("hit.x",&fGenDetHit_X);
  Tmol->SetBranchAddress("hit.y",&fGenDetHit_Y);
  Tmol->SetBranchAddress("hit.z",&fGenDetHit_Z);
  Tmol->SetBranchAddress("hit.vx",&fGenDetHit_VX);
  Tmol->SetBranchAddress("hit.vy",&fGenDetHit_VY);
  Tmol->SetBranchAddress("hit.vz",&fGenDetHit_VZ);
  Int_t nentries = (Int_t)Tmol->GetEntries();

  if (kSaveRootFile){
    TString rootfilestatus="RECREATE";
    rootfile = new TFile(rootfilename, rootfilestatus);
    rootfile->cd();
  }

  set_plot_style();

  gROOT->SetStyle("Plain");
  //gStyle->SetOptStat(0); 
  gStyle->SetOptStat("eMR");
  gStyle->SetNumberContours(255);

  //indices asigned to each detector
  detectormap[6]=1;     // Roof det
  detectormap[9]=2;     // Dump det
  detectormap[53]=3;    // Gatevalve det
  detectormap[100]=0;   // dumpDet

  //indices asigned to pid numbers
  pidmap[11]=0; //electron 
  pidmap[22]=1; //photon
  pidmap[2112]=2; //neutron

  pidmass[11]=0.511;//MeV
  pidmass[22]=0.0;
  pidmass[2112]=939.565;//MeV

  TH1D *HistoE_RadDet[n_detectors][n_particles];
  TH2D *HistoVertex_RadDet[n_detectors][n_particles];
  TH2D *HistoHit_RadDet[n_detectors][n_particles];

  TString spid[n_particles]={"e+-","#gamma","n0"};
  int n_bins[4][n_particles]={
    {3000,3000,750},
    {200,200,50},
    {400,400,100},
    {400,400,100}};

  for(Int_t i=0;i<n_detectors;i++){//vertices
    for(Int_t j=0;j<n_particles;j++){//particles
      HistoE_RadDet[i][j]=new TH1D(Form("HistoE_RadDet_det%d_p%d",i+1,j+1),Form(" %s into %s det; E (MeV); (Counts)",spid[j].Data(),sdet[i].Data()),100,0.1,300);
      HistoVertex_RadDet[i][j]=new TH2D(Form("HistoVertex_RadDet_det%d_p%d",i+1,j+1),Form(" %s into %s det; z (cm); y (cm); E (MeV)",spid[j].Data(),sdet[i].Data()),n_bins[0][j],-200.,5800.,n_bins[1][j],-250.,150.);
      HistoHit_RadDet[i][j]=new TH2D(Form("HistoHit_RadDet_det%d_p%d",i+1,j+1),Form(" %s into %s det; x (cm); y (cm); E (MeV)",spid[j].Data(),sdet[i].Data()),n_bins[2][j],-400.,400.,n_bins[3][j],-400.,400.);
    }
  }

  int detid    = -1;
  int pid      = -1;

  //printf("Normalized to %d events \n",n_events);
  for (int i=0; i<nentries ; i++) {
    Tmol->GetEntry(i);
    for (int j = 0; j<fNGenDetHit; j++){
      if(kVertices && fGenDetHit_PZ[j]<=0.0 && (fGenDetHit_X[j]<=2.0 &&fGenDetHit_X[j]>=-2.0) && (fGenDetHit_Y[j]<=2.0 && fGenDetHit_Y[j]>=-2.0) && (fGenDetHit_det[j]==SensVolume_v[0] || fGenDetHit_det[j]==SensVolume_v[1] || fGenDetHit_det[j]==SensVolume_v[2] || fGenDetHit_det[j]==SensVolume_v[3]) && (TMath::Abs(fGenDetHit_pid[j])==11 || fGenDetHit_pid[j]==22 || fGenDetHit_pid[j]==2112) ){//total into the hall
	      //big set of for loops!!
	      detid=detectormap[fGenDetHit_det[j]]; 
        pid=pidmap[(Int_t)TMath::Abs(fGenDetHit_pid[j])];
        kineE = TMath::Sqrt(TMath::Power(fGenDetHit_P[j]*1000,2) + TMath::Power(pidmass[(Int_t)TMath::Abs(fGenDetHit_pid[j])],2) ) - pidmass[(Int_t)TMath::Abs(fGenDetHit_pid[j])];
        if (kineE >= 0.1){
          flux_local[detid][pid]++;
          power_local[detid][pid]+=kineE;
          HistoE_RadDet[detid][pid]->Fill(kineE,1);
          HistoVertex_RadDet[detid][pid]->Fill(fGenDetHit_VZ[j]*100,fGenDetHit_VY[j]*100,kineE);
          HistoHit_RadDet[detid][pid]->Fill(fGenDetHit_X[j]*100,fGenDetHit_Y[j]*100,kineE);
        }
      }
    }

    if (i>200000 && earlybreak)
      break;
    if (i%500000==0)
      printf("Event %d of %d \n",i,nentries);
  }
  
  // FIXME Drawing and Saving Loops

  //2D radiation vertex distr 
  if (kShow2DVertexPlots){
    Double_t hallrad_vert_color_max;
    Double_t hallrad_vert_color_min;
    Double_t hallrad_hit_color_max;
    Double_t hallrad_hit_color_min;
    Double_t hallrad_E_color_max;
    Double_t hallrad_E_color_min;
    for(Int_t i=0;i<n_detectors;i++){//region
      for(Int_t j=0;j<n_particles;j++){//pid
        //HistoVertex_RadDet[i][j]->Scale(1e9);
     //   if (HistoE_RadDet[i][j]->GetMinimum()<hallrad_E_color_min) hallrad_E_color_min = HistoE_RadDet[i][j]->GetMinimum();
     //   if (HistoE_RadDet[i][j]->GetMaximum()>hallrad_E_color_max) hallrad_E_color_max = HistoE_RadDet[i][j]->GetMaximum();
        if (HistoHit_RadDet[i][j]->GetMinimum()<hallrad_hit_color_min) hallrad_hit_color_min = HistoHit_RadDet[i][j]->GetMinimum();
        if (HistoHit_RadDet[i][j]->GetMaximum()>hallrad_hit_color_max) hallrad_hit_color_max = HistoHit_RadDet[i][j]->GetMaximum();
        if (HistoVertex_RadDet[i][j]->GetMinimum()<hallrad_vert_color_min) hallrad_vert_color_min = HistoVertex_RadDet[i][j]->GetMinimum();	
        if (HistoVertex_RadDet[i][j]->GetMaximum()>hallrad_vert_color_max) hallrad_vert_color_max = HistoVertex_RadDet[i][j]->GetMaximum();	
      }
    }
    TCanvas * canvas_hallrad_Ee[n_detectors];
    TCanvas * canvas_hallrad_En[n_detectors];
    TCanvas * canvas_hallrad_Eg[n_detectors];
    TCanvas * canvas_hallrad_xy_hit[n_detectors];
    TCanvas * canvas_hallrad_yz_vrtx[n_detectors];
    for(Int_t i=0;i<n_detectors;i++){//vertex
      canvas_hallrad_Ee[i]=new TCanvas(Form("canvas_%s_hallrad_E_electrons_%02d",argv[1],i+1),Form("canvas_%s_hallrad_E_electrons_%02d",argv[1],i+1),1500,1500);
      canvas_hallrad_Eg[i]=new TCanvas(Form("canvas_%s_hallrad_E_photons_%02d",argv[1],i+1),Form("canvas_%s_hallrad_E_photons_%02d",argv[1],i+1),1500,1500);
      canvas_hallrad_En[i]=new TCanvas(Form("canvas_%s_hallrad_E_neutrons_%02d",argv[1],i+1),Form("canvas_%s_hallrad_E_neutrons_%02d",argv[1],i+1),1500,1500);
      canvas_hallrad_xy_hit[i]=new TCanvas(Form("canvas_%s_hallrad_xy_hit%02d",argv[1],i+1),Form("canvas_%s_hallrad_xy_hit%02d",argv[1],i+1),1500,1500);
      canvas_hallrad_yz_vrtx[i]=new TCanvas(Form("canvas_%s_hallrad_yz_vrtx%02d",argv[1],i+1),Form("canvas_%s_hallrad_yz_vrtx%02d",argv[1],i+1),1500,1500);
      canvas_hallrad_xy_hit[i]->Divide(n_particles,1); 
      canvas_hallrad_yz_vrtx[i]->Divide(1,n_particles);
          
      canvas_hallrad_Ee[i]->cd();
      canvas_hallrad_Ee[i]->cd()->SetLogy();
      HistoE_RadDet[i][0]->Draw();// THESE ARE THE vertices of particle birth plots	
    //HistoE_RadDet[i][0]->GetYaxis()->SetRangeUser(hallrad_E_color_min,hallrad_E_color_max);
      HistoE_RadDet[i][0]->SetStats(1);

      canvas_hallrad_Eg[i]->cd();
      canvas_hallrad_Eg[i]->cd()->SetLogy();
      HistoE_RadDet[i][1]->Draw();// THESE ARE THE vertices of particle birth plots	
    //HistoE_RadDet[i][1]->GetYaxis()->SetRangeUser(hallrad_E_color_min,hallrad_E_color_max);
      HistoE_RadDet[i][1]->SetStats(1);

      canvas_hallrad_En[i]->cd();
      canvas_hallrad_En[i]->cd()->SetLogy();
      HistoE_RadDet[i][2]->Draw();// THESE ARE THE vertices of particle birth plots	
    //HistoE_RadDet[i][2]->GetYaxis()->SetRangeUser(hallrad_E_color_min,hallrad_E_color_max);
      HistoE_RadDet[i][2]->SetStats(1);

      for(Int_t j=0;j<n_particles;j++){//pid
          canvas_hallrad_xy_hit[i]->cd(j+1);
          canvas_hallrad_xy_hit[i]->cd(j+1)->SetLogz();
          HistoHit_RadDet[i][j]->Draw("colz");// THESE ARE THE vertices of particle birth plots	
          HistoHit_RadDet[i][j]->GetZaxis()->SetRangeUser(hallrad_hit_color_min,hallrad_hit_color_max);
          HistoHit_RadDet[i][j]->SetStats(0);

          canvas_hallrad_yz_vrtx[i]->cd(j+1);           
          canvas_hallrad_yz_vrtx[i]->cd(j+1)->SetLogz();
          HistoVertex_RadDet[i][j]->Draw("colz");	
          HistoVertex_RadDet[i][j]->GetZaxis()->SetRangeUser(hallrad_vert_color_min,hallrad_vert_color_max);
          HistoVertex_RadDet[i][j]->SetStats(0);
        }
      if (kSave2DHisto){
	      canvas_hallrad_Ee[i]->SaveAs(plotsFolder+Form("canvas_%s_hallrad_E_electrons_%02d.png",argv[1],i+1));
	      canvas_hallrad_Eg[i]->SaveAs(plotsFolder+Form("canvas_%s_hallrad_E_photons_%02d.png",argv[1],i+1));
	      canvas_hallrad_En[i]->SaveAs(plotsFolder+Form("canvas_%s_hallrad_E_neutrons_%02d.png",argv[1],i+1));
	      canvas_hallrad_xy_hit[i]->SaveAs(plotsFolder+Form("canvas_%s_hallrad_xy_hit%02d.png",argv[1],i+1));
	      canvas_hallrad_yz_vrtx[i]->SaveAs(plotsFolder+Form("canvas_%s_hallrad_yz_vrtx%02d.png",argv[1],i+1));
      }
      if (kSaveRootFile){
      	canvas_hallrad_Ee[i]->Write();
      	canvas_hallrad_Eg[i]->Write();
      	canvas_hallrad_En[i]->Write();
      	canvas_hallrad_xy_hit[i]->Write();
        canvas_hallrad_yz_vrtx[i]->Write();
      }
    }
  }

  // Textfile outputs
  const char * chpid[n_particles]         = {"e+-","photon","n0"};

  TList * list_power = new TList;
  TString strline;
  char line[600];
  char line1[600];
  strline="Rootfile_name";
  list_power->Add(new TObjString(strline));
  list_outputs << strline << endl;
  //strline=added_file;
  strline=added_file_array[1];
  list_power->Add(new TObjString(strline));
  list_outputs << strline << endl;
  // POWER
  strline="Total_Radiation_Power_into_the_specified_detector_(MeV)";
  list_power->Add(new TObjString(strline));
  list_outputs << strline << endl;
  printf(" \n Total_Radiation_Power_into_the_specified_detector_(MeV) \n");
  printf(" %20s","Type");
  sprintf(line," %20s","Type");
  printf(" \n");
  list_power->Add(new TObjString(line));
  list_outputs << line << endl;
  Double_t sum=0;
 
  // FIXME Add a variable that gets += every time a unique count of radiation gets printed in order to sum up all the radiation being produced/absorbed across all regions
  for(Int_t i=0;i<n_particles;i++){//pid
    printf(" %20s",chpid[i]);
    sprintf(line," %20s",chpid[i]);
    sprintf(line1," ");//empty previous values
    for(Int_t k=0;k<n_detectors;k++){//detector  // Suming over detectors doesn't really serve a purpose.....
 	      sum+=power_local[k][i];
        printf("%12.3E",sum);///n_events);
	      sprintf(line1,"%s %12.3E",line1,sum);///n_events);
	      sum=0;
    }
    printf("\n");
    sprintf(line," %s %s",line,line1);
    list_power->Add(new TObjString(line));
    list_outputs << line << endl;
  }

  // FLUX  
  sum=0;
  TList * list_flux = new TList;
  printf(" \n Total_Radiation_Flux_into_the_specified_detector_(Counts)\n");
  strline="Total_Radiation_Flux_into_the_speicified_detector_(Counts)";
  list_flux->Add(new TObjString(strline));
  list_outputs << strline << endl;
  printf(" %20s","Type");
  sprintf(line," %20s","Type");
  printf(" \n");
  list_flux->Add(new TObjString(line));
  list_outputs << line << endl;
  for(Int_t i=0;i<n_particles;i++){//pid
    printf(" %20s",chpid[i]);
    sprintf(line," %20s",chpid[i]);
    sprintf(line1," ");//empty previous values
    for(Int_t k=0;k<n_detectors;k++){//detector
	    sum+=flux_local[k][i];
      printf("%12.3E",sum);//*6.241e+12/n_events);
	    sprintf(line1,"%s %12.3E",line1,sum);//*6.241e+12/n_events);
	    sum=0;
    }
    printf("\n");
    sprintf(line," %s %s",line,line1);
    list_flux->Add(new TObjString(line));
    list_outputs << line << endl;
  }
  
  if (kSaveRootFile){
    rootfile->WriteObject(list_power,"textout_power");
    rootfile->WriteObject(list_flux,"textout_flux");
    rootfile->Write();
  }

  if(kShowGraphic)
    theApp.Run();

  if (kSaveRootFile){
    rootfile->Close();
  }

  list_outputs.close();
  return(1);
}

void set_plot_style()
{
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    // See class TColor documentation and SetPalette() command
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}
