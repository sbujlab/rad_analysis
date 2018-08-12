// Cameron Clarke 8/9/2018 - Based off of Rakitha's rad_dose.cc


#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>

#include <TApplication.h>
#include <TRint.h>
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

#include "remolltypes.hh"
using namespace std;



//Double_t fEvRate;

//Detector numbers (wall, ceiling, floor, hall, lead target hut, poly target hut, lead collar, poly collar, block 1, block 2, blocks 1 and 2 poly shield, block 3, block 3's poly shield, hybrid concrete hut, hybrid poly hut, hybrid lead roof)//look at everything going out to the hall
Int_t SensVolume_v[] = {99,101,103,6000,6003,6004,6007,6008,6010,6011,6012,6020,6021,6027,6028,6030};
const int n_energy_ranges = 3;
const int n_particles = 3;

const int n_regions = 6; // originally 6, used for mapping localized radiation to the whole hall
Double_t flux_local[n_regions+1][n_particles][n_energy_ranges]={{{0}}}; // The last index is for the shieldings: target, shielding blocks 1 to 4, and other vertices
Double_t power_local[n_regions+1][n_particles][n_energy_ranges]={{{0}}};

Int_t detector;
std::map<int,int> detectormap;
std::map<int,int> pidmap;

Bool_t kSaveRootFile=kTRUE;           //save histograms and canvases into a rootfile


void set_plot_style();

TFile * rootfile;
int main(int argc,char** argv) {
    TApplication theApp("App",&argc,argv);
    //std::vector < remollGenericDetectorHit_t > *fGenDetHitHelper = new std::vector < remollGenericDetectorHit_t >; 
    //Double_t fEvRate;
    ofstream list_outputs;

    //remoll Tree
    TChain * Tmol =new TChain("T");
    const int n_mills = 10;// FIXME number of million events

    Int_t n_events = n_mills*1e6;
    Int_t beamcurrent = 85;//uA

    TString added_file_array[n_mills]={""};//n_mills]={""}; // The last index is for the shieldings: target, shielding blocks 1 to 4, and other vertices
    for (int v=1 ; v <= n_mills ; v++){ 
        ostringstream temp_str_stream2;
        ostringstream temp_str_stream3;
        temp_str_stream2<<v;
        TString vS;
        vS=temp_str_stream2.str();
        temp_str_stream3<<"/home/cameronc/gitdir/remoll/output/"<<argv[1]<<"_"<<n_mills<<"M/out_"<<argv[1]<<vS<<"/remoll_"<<argv[1]<<"_1M_optimized_det101.root";
        added_file_array[v]=temp_str_stream3.str();
        Tmol->Add(added_file_array[v]);
    }

    ostringstream temp_str_stream4;
    temp_str_stream4<<"/home/cameronc/gitdir/remoll/output/Plots_"<<argv[1]<<"_"<<n_mills<<"M/";//Name of folder for saving plots
    TString plotsFolder=temp_str_stream4.str();//Name of folder for saving plots

    ostringstream temp_str_stream5;
    temp_str_stream5<<plotsFolder<<argv[1]<<"_"<<n_mills<<"M_plots.root";//name of the rootfile to save generated histograms
    TString rootfilename=temp_str_stream5.str();//name of the rootfile to save generated histograms

    ostringstream temp_str_stream6;
    temp_str_stream6<<plotsFolder<<"list_outputs_"<<argv[1]<<"_"<<n_mills<<"M.txt";
    TString textfilename=temp_str_stream6.str();
    list_outputs.open(textfilename);

    list_outputs << "Contents of textout_flux and textout_power lists of strings" << std::endl;

    //Tmol->SetBranchAddress("rate",&fEvRate);
    //Tmol->SetBranchAddress("hit",&fGenDetHitHelper);
    Int_t n_entries=n_events;//Tmol->GetEntries();

    const Int_t nentries = (Int_t)Tmol->GetEntries();
    printf("Normalized to %d events \n",n_entries);

    if (kSaveRootFile){
        TString rootfilestatus="RECREATE";
        rootfile = new TFile(rootfilename, rootfilestatus);
        rootfile->cd();
    }


    gROOT->SetStyle("Plain");
    //gStyle->SetOptStat(0); 
    gStyle->SetOptStat("eMR");
    gStyle->SetNumberContours(255);

    set_plot_style();

    detector=101;
    //  //indices asigned to each detector
    //  detectormap[99]=0;    // Cyl det
    //  detectormap[101]=1;   // Roof
    //  detectormap[103]=2;   // Floor

    //indices asigned to pid numbers
    pidmap[0]=11; //electron 
    pidmap[1]=22; //photon
    pidmap[2]=2112; //neutron

    // Declaring histograms - counts and energy as a function of z vertex position
    TH1F *Histo_kineE_spectrum[n_regions+1][n_particles][n_energy_ranges]; // only one that was originally n_regions+1
    TH1F *Histo_kineE_vertices[n_regions+1][n_particles][n_energy_ranges]; // only one that was originally n_regions+1
    TH1F *Histo_counts_vertex[n_regions+1][n_particles][n_energy_ranges];
    TH2D *HistoVertex_RadDet_side[n_regions+1][n_particles][n_energy_ranges];//n_regions vertex ranges for three particles species for three energy ranges 
    TH2D *HistoVertex_RadDet_roof[n_regions+1][n_particles][n_energy_ranges];//n_regions vertex ranges for three particles species for three energy ranges 


    //                                      { change the binning to reflect the opposite nature, good, all binned in one spot, decent-needs better boundaries->775?, 775?, decent, decent, seems to miss a whole lot }
    //                                      { begin-front, target, coll1shld, coll4shld, hybshld, back-end  }; -> Hall is an inverted volume in x and z, not y.
    Double_t z_vertex_cuts[n_regions+1] = {-20000.0, -3170., 3151., 8030.0,  9930., 18218.37, 35000.}; //last index store vertices outside of other ranges 
    Int_t z_vertex_bin_counts[n_regions]={0};
    Int_t z_area_per_bin = 10; // 10 mm per bin
    for (int q=0;q<n_regions;q++){
        z_vertex_bin_counts[q] = (z_vertex_cuts[q+1]-z_vertex_cuts[q])/z_area_per_bin;
    }

    Int_t energy_ranges[n_energy_ranges+1]={0,10,25,100000};
    Int_t energy_bin_ranges[n_energy_ranges+1]={0,10,25,10000};

    TString ke_range[n_energy_ranges] = {"KE<10","10<KE<25","25<KE"};
    TString spid[n_particles]={"e+-","photon","n0"};
    TString svertex[n_regions+1]={"Front","Target","Col1Shld","Coll4Shld","HybridShld","Downstream","All"};     

    TList * list = new TList;
    Int_t counts[n_regions+1][n_particles][n_energy_ranges] = {{{0}}};
    Double_t energy[n_regions+1][n_particles][n_energy_ranges] = {{{0.}}};
    TString strline;
    char line[600];
    char line1[600];
    char line2[600];
    strline="Rootfile_name";
    list->Add(new TObjString(strline));
    list_outputs << strline << endl;
    //strline=added_file;
    strline=added_file_array[1];
    list->Add(new TObjString(strline));
    list_outputs << strline << endl;
    TCanvas * c1[5][n_regions];

    printf("\nTotal_Radiation_Flux_into_the_Roof_(Counts/n_events) \n");
    strline="Total_Radiation_Flux_into_the_Roof_(Counts/n_events)";
    list->Add(new TObjString(strline));
    list_outputs << strline << endl;
    printf("%20s %20s %20s %20s %20s \n","Region","Type","E_Range_(MeV)","Counts","Energy");
    sprintf(line,"%20s %20s %20s %20s %20s","Region","Type","E_Range_(MeV)","Counts","Energy");
    list->Add(new TObjString(line));
    list_outputs << line << endl;

    for(int i=0;i<n_regions;i++){//vertices

        c1[0][i]=new TCanvas(Form("canvas_hallrad_z_vrtx_kineEweighted_region%02d",i+1),Form("canvas_hallrad_z_vrtx_kineEweighted_region%02d",i+1),1500,1500);
        c1[0][i]->Divide(n_particles,n_energy_ranges); 
        for(int j=0;j<n_particles;j++){//pid
            for(int k=0;k<n_energy_ranges;k++){//KE
                c1[0][i]->cd(n_energy_ranges*j+1+k);
                //1D radiation histograms
                Histo_kineE_vertices[i][j][k]=new TH1F(Form("Histo_kineE_vertices[%d][%d][%d]",i,j,k),Form("%s from %s Area in %s MeV Range; Z Vertices (mm)",spid[j].Data(),svertex[i].Data(),ke_range[k].Data()),z_vertex_bin_counts[i],z_vertex_cuts[i]-1,z_vertex_cuts[i+1]+1);
                Tmol->Draw(Form("hit.vz>>Histo_kineE_vertices[%d][%d][%d]",i,j,k),Form("(hit.e-hit.m)*(hit.vz > %f && hit.vz <= %f && hit.pid==%d && (hit.e-hit.m) > %d && (hit.e-hit.m) <= %d)",z_vertex_cuts[i],z_vertex_cuts[i+1],pidmap[j],energy_ranges[k],energy_ranges[k+1]));
                counts[i][j][k] = 1.0*Histo_kineE_vertices[i][j][k]->GetEntries();
                energy[i][j][k] = Histo_kineE_vertices[i][j][k]->Integral();
                counts[n_regions][j][k] += 1.0*Histo_kineE_vertices[i][j][k]->GetEntries();
                energy[n_regions][j][k] += Histo_kineE_vertices[i][j][k]->Integral();
                printf("%20s %20s %20s",svertex[i].Data(),spid[j].Data(),ke_range[k].Data());
                sprintf(line,"%20s %20s %20s",svertex[i].Data(),spid[j].Data(),ke_range[k].Data());
                sprintf(line1," ");//empty previous values
                printf(" %12.3E %12.3E \n",(1.0*Histo_kineE_vertices[i][j][k]->GetEntries())/n_entries,energy[i][j][k]/n_entries);
                sprintf(line1,"%s %12.3E %12.3E ",line1,(1.0*Histo_kineE_vertices[i][j][k]->GetEntries())/n_entries,energy[i][j][k]/n_entries);
                sprintf(line," %s %s",line,line1);
                list->Add(new TObjString(line));
                list_outputs << line << endl;
                if (i==n_regions-1) {
                    printf("Total %20s %20s",spid[j].Data(),ke_range[k].Data());
                    sprintf(line,"Total %20s %20s",spid[j].Data(),ke_range[k].Data());
                    sprintf(line1," ");//empty previous values
                    printf(" %12.3E %12.3E \n",1.0*counts[n_regions][j][k]/n_entries,energy[n_regions][j][k]/n_entries);
                    sprintf(line1,"%s %12.3E %12.3E ",line1,1.0*counts[n_regions][j][k]/n_entries,energy[n_regions][j][k]/n_entries);
                    sprintf(line," %s %s",line,line1);
                    list->Add(new TObjString(line));
                    list_outputs << line << endl;
                }
            }
        }
        c1[0][i]->Write();
        c1[0][i]->SaveAs(plotsFolder+Form("canvas_hallrad_z_vrtx_kineEweighted_region%02d.png",i+1));
        c1[1][i]=new TCanvas(Form("canvas_hallrad_energy_spectrum_region%02d",i+1),Form("canvas_hallrad_energy_spectrum_region%02d",i+1),1500,1500);
        c1[1][i]->Divide(n_particles,n_energy_ranges); 
        for(int j=0;j<n_particles;j++){//pid
            for(int k=0;k<n_energy_ranges;k++){//KE
                c1[1][i]->cd(n_energy_ranges*j+1+k);
                Histo_kineE_spectrum[i][j][k] = new TH1F(Form("Histo_kineE_spectrum[%d][%d][%d]",i,j,k),Form("%s from %s Area in %s MeV Range; KineE Spectrum in MeV",spid[j].Data(),svertex[i].Data(),ke_range[k].Data()),100,energy_bin_ranges[k],energy_bin_ranges[k+1]);
                Tmol->Draw(Form("(hit.e-hit.m)>>+Histo_kineE_spectrum[%d][%d][%d]",i,j,k),Form("hit.vz > %f && hit.vz <= %f && hit.pid==%d && (hit.e-hit.m) > %d && (hit.e-hit.m) <= %d",z_vertex_cuts[i],z_vertex_cuts[i+1],pidmap[j],energy_ranges[k],energy_ranges[k+1]));

            }
        }
        c1[1][i]->Write();
        c1[1][i]->SaveAs(plotsFolder+Form("canvas_hallrad_energy_spectrum_region%02d.png",i+1));
        c1[2][i]=new TCanvas(Form("canvas_hallrad_z_vrtx_unweighted_region%02d",i+1),Form("canvas_hallrad_z_vrtx_unweighted_region%02d",i+1),1500,1500);
        c1[2][i]->Divide(n_particles,n_energy_ranges); 
        for(int j=0;j<n_particles;j++){//pid
            for(int k=0;k<n_energy_ranges;k++){//KE
                c1[2][i]->cd(n_energy_ranges*j+1+k);
                Histo_counts_vertex[i][j][k]=new TH1F(Form("Histo_counts_vertices[%d][%d][%d]",i,j,k),Form("%s Vertices from %s Area in %s MeV Range;Z vertex (mm);MeV",spid[j].Data(),svertex[i].Data(),ke_range[k].Data()),z_vertex_bin_counts[i],z_vertex_cuts[i]-1,z_vertex_cuts[i+1]+1);
                Tmol->Draw(Form("hit.vz>>Histo_counts_vertex[%d][%d][%d]",i,j,k),Form("hit.vz > %f && hit.vz <= %f && hit.pid==%d && (hit.e-hit.m) > %d & (hit.e-hit.m) <= %d",z_vertex_cuts[i],z_vertex_cuts[i+1],pidmap[j],energy_ranges[k],energy_ranges[k+1]));
            }
        }
        c1[2][i]->Write();
        c1[2][i]->SaveAs(plotsFolder+Form("canvas_hallrad_z_vrtx_unweighted_region%02d.png",i+1));
        c1[3][i]=new TCanvas(Form("canvas_hallrad_yz_hits_region%02d",i+1),Form("canvas_hallrad_yz_hits_region%02d",i+1),1500,1500);
        c1[3][i]->Divide(n_particles,n_energy_ranges); 
        for(int j=0;j<n_particles;j++){//pid
            for(int k=0;k<n_energy_ranges;k++){//KE
                //2D vertex distribution histograms
                c1[3][i]->cd(n_energy_ranges*j+1+k);
                HistoVertex_RadDet_side[i][j][k]=new TH2D(Form("HistoVertex_RadDet_side[%d][%d][%d]",i,j,k),Form("Side view %s Vertices from %s Area in %s MeV Range; y (mm); z (mm); (Counts)",spid[j].Data(),svertex[i].Data(),ke_range[k].Data()),350,-3500.0,3500.0,z_vertex_bin_counts[i],z_vertex_cuts[i]-1,z_vertex_cuts[i+1]+1);
                Tmol->Draw(Form("hit.vy:hit.vz>>HistoVertex_RadDet_side[%d][%d][%d]",i,j,k),Form("(hit.vz > %f && hit.vz <= %f && hit.pid==%d && (hit.e-hit.m) > %d && (hit.e-hit.m) <= %d)",z_vertex_cuts[i],z_vertex_cuts[i+1],pidmap[j],energy_ranges[k],energy_ranges[k+1]),"COLZ");
            }
        }
        c1[3][i]->Write();
        c1[3][i]->SaveAs(plotsFolder+Form("canvas_hallrad_yz_vrtx_region%02d.png",i+1));
        c1[4][i]=new TCanvas(Form("canvas_hallrad_xy_hits_region%02d",i+1),Form("canvas_hallrad_xy_hits_region%02d",i+1),1500,1500);
        c1[4][i]->Divide(n_particles,n_energy_ranges); 
        for(int j=0;j<n_particles;j++){//pid
            for(int k=0;k<n_energy_ranges;k++){//KE
                c1[4][i]->cd(n_energy_ranges*j+1+k);
                HistoVertex_RadDet_roof[i][j][k]=new TH2D(Form("HistoVertex_RadDet_roof[%d][%d][%d]",i,j,k),Form("Roof hit %s Positions from %s Area in %s MeV Range; x (mm); z (mm); (Counts)",spid[j].Data(),svertex[i].Data(),ke_range[k].Data()),350,-3500.0,3500.0,z_vertex_bin_counts[i],z_vertex_cuts[i]-1,z_vertex_cuts[i+1]+1);
                Tmol->Draw(Form("hit.x:hit.z>>HistoVertex_RadDet_roof[%d][%d][%d]",i,j,k),Form("(hit.vz > %f && hit.vz <= %f && hit.pid==%d && (hit.e-hit.m) > %d && (hit.e-hit.m) <= %d)",z_vertex_cuts[i],z_vertex_cuts[i+1],pidmap[j],energy_ranges[k],energy_ranges[k+1]),"COLZ");
            }
        }
        c1[4][i]->Write();
        c1[4][i]->SaveAs(plotsFolder+Form("canvas_hallrad_xy_hits_region%02d.png",i+1));
    } 
    if (kSaveRootFile){
        rootfile->WriteObject(list,"text_output");
        rootfile->Write();
        rootfile->Close();
    }

    theApp.Run();
    list_outputs.close();
    return(0);
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
