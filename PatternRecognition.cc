/**************************************************************
 * Associative Memory Emulation Software for L1 Track Trigger *
 * Pattern Recognition Module                                 *
 * Authors: Souvik Das                                        *
 *************************************************************/

#include <iostream>
#include <unordered_map>

#include <TFile.h>
#include <TChain.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TStopwatch.h>

#include "Pattern.h"
#include "Coordinator.h"

int main()
{
  // Open a TChain to hold the Pattern Bank
  TChain *bank = new TChain("PatternBank");
  bank->Add("PatternBank.root");
  
  // Open a TChain to hold the contents of the ROOT file containing stubs
  TChain *tree = new TChain("L1TrackTrigger");
  tree->Add("/Users/souvik/CMSPhase2Upgrades/Samples/SLHC/GEN/612_SLHC6_MU/MU_612_SLHC6.root");
  
  // Open a TChain to hold the generator level information
  TChain *MC = new TChain("MC");
  MC->Add("/Users/souvik/CMSPhase2Upgrades/Samples/SLHC/GEN/612_SLHC6_MU/MU_612_SLHC6.root");
  
  // Fill Pattern Bank
  int nLayersOnAMChip=layersOnChip;
  int popularity;
  int t_pattern[layersOnChip];
  bank->SetBranchAddress("nLayersOnAMChip", &nLayersOnAMChip);
  bank->SetBranchAddress("popularity", &popularity);
  bank->SetBranchAddress("pattern", &t_pattern);
  std::vector<PatternPrimitive> patternBank;
  std::unordered_map<int, SuperStrip*> superStripMap;
  for (unsigned int i=0; i<bank->GetEntries(); ++i)
  {
    bank->GetEvent(i);
    if (popularity>0)
    {
      PatternPrimitive pattern;
      for (int j=0; j<nLayersOnAMChip; ++j)
      {
        int superStripID=t_pattern[j];
        pattern.push_back(superStripID);
        if (superStripMap.find(superStripID)==superStripMap.end())
        {
          SuperStrip *superStrip=new SuperStrip(superStripID);
          superStripMap.insert(std::pair<int, SuperStrip*>(superStripID, superStrip));
          // std::cout<<"Made a new superstrip with id = "<<superStripID<<" at address = "<<superStripMap[superStripID]<<std::endl;
        }
      }
      patternBank.push_back(pattern);
    }
  }
  std::cout<<"Filled Pattern Bank in memory."<<std::endl;
  
  // Declare and assign variables for the stub information
  int STUB_n;
  std::vector<int> *STUB_layer=0;
  std::vector<int> *STUB_ladder=0;
  std::vector<int> *STUB_module=0;
  std::vector<int> *STUB_seg=0;
  std::vector<int> *STUB_strip=0;
  std::vector<float> *STUB_x=0;
  std::vector<float> *STUB_y=0;
  std::vector<float> *STUB_z=0;
  tree->SetBranchAddress("STUB_n", &STUB_n);
  tree->SetBranchAddress("STUB_layer", &STUB_layer);
  tree->SetBranchAddress("STUB_ladder", &STUB_ladder);
  tree->SetBranchAddress("STUB_module", &STUB_module);
  tree->SetBranchAddress("STUB_seg", &STUB_seg);
  tree->SetBranchAddress("STUB_strip", &STUB_strip);
  tree->SetBranchAddress("STUB_x", &STUB_x);
  tree->SetBranchAddress("STUB_y", &STUB_y);
  tree->SetBranchAddress("STUB_z", &STUB_z);
  
  int gen_n;
  std::vector<float> *gen_px=0;
  std::vector<float> *gen_py=0;
  std::vector<float> *gen_pz=0;
  MC->SetBranchAddress("gen_n", &gen_n);
  MC->SetBranchAddress("gen_px", &gen_px);
  MC->SetBranchAddress("gen_py", &gen_py);
  MC->SetBranchAddress("gen_pz", &gen_pz);
  
  // Efficiency plots
  TH1F *h_eff_eta_num=new TH1F("h_eff_eta_num", "h_eff_eta_num", 100, -2.5, 2.5);
  TH1F *h_eff_eta_den=new TH1F("h_eff_eta_den", "h_eff_eta_den", 100, -2.5, 2.5);
  TH1F *h_eff_phi_num=new TH1F("h_eff_phi_num", "h_eff_phi_num", 100, -3.14, 3.14);
  TH1F *h_eff_phi_den=new TH1F("h_eff_phi_den", "h_eff_phi_den", 100, -3.14, 3.14);
  TH1F *h_eff_pT_num=new TH1F("h_eff_pT_num", "h_eff_pT_num", 100, 0., 100.);
  TH1F *h_eff_pT_den=new TH1F("h_eff_pT_den", "h_eff_pT_den", 100, 0., 100.);
  TH2F *h_eff_eta_phi_num=new TH2F("h_eff_eta_phi_num", "h_eff_eta_phi_num", 100, -2.5, 2.5, 100, -3.14, 3.14);
  TH2F *h_eff_eta_phi_den=new TH2F("h_eff_eta_phi_den", "h_eff_eta_phi_den", 100, -2.5, 2.5, 100, -3.14, 3.14);
  TH2F *h_eff_px_py_num=new TH2F("h_eff_px_py_num", "h_eff_px_py_num", 100, -100., 100., 100, -100., 100.);
  TH2F *h_eff_px_py_den=new TH2F("h_eff_px_py_den", "h_eff_px_py_den", 100, -100., 100., 100, -100., 100.);
  
  // Event loop
  int nEvents=tree->GetEntries();
  TStopwatch w_Event, w_loadHits, w_litPatterns, w_eraseHits;
  w_Event.Start();
  nEvents=500;
  for (int i=0; i<nEvents; ++i)
  {
    tree->GetEvent(i);
    MC->GetEvent(i);
    
    // Light up superstrips according to stubs in the event
    w_loadHits.Start(false);
    for (unsigned int j=0; j<STUB_layer->size(); ++j)
    {
      int superStripCoordinates=superStripCoordinate(STUB_layer->at(j), STUB_ladder->at(j), STUB_module->at(j), STUB_seg->at(j), STUB_strip->at(j));
      std::unordered_map<int, SuperStrip*>::iterator itr=superStripMap.find(superStripCoordinates);
      if (itr!=superStripMap.end()) itr->second->setStatus(1);
      // else std::cout<<"SuperStrip "<<superStripCoordinates<<" doesn't exist in Pattern Bank"<<std::endl;
    }
    w_loadHits.Stop();
    
    // Find patterns that lit up
    TVector3 gen_p3(gen_px->at(0), gen_py->at(0), gen_pz->at(0));
    h_eff_eta_den->Fill(gen_p3.Eta());
    h_eff_phi_den->Fill(gen_p3.Phi());
    h_eff_pT_den->Fill(gen_p3.Pt());
    h_eff_eta_phi_den->Fill(gen_p3.Eta(), gen_p3.Phi());
    h_eff_px_py_num->Fill(gen_p3.X(), gen_p3.Y());
    w_litPatterns.Start(false);
    bool foundPattern=false;
    for (unsigned int j=0; j<patternBank.size(); ++j)
    {
      int lights=0;
      for (unsigned int k=0; k<layersOnChip; ++k)
      {
        if (patternBank.at(j).at(k)!=0)
        {
          if (superStripMap.find(patternBank.at(j).at(k))==superStripMap.end()) std::cout<<"Superstrip in pattern doesn't exist in superstrip map"<<std::endl;
          else lights+=superStripMap[patternBank.at(j).at(k)]->getStatus();
        } else lights+=1;
      }
      // A pattern is found!
      if (lights==8)
      {
        foundPattern=true;
      }
    }
    w_litPatterns.Stop();
    if (foundPattern)
    {
      h_eff_eta_num->Fill(gen_p3.Eta());
      h_eff_phi_num->Fill(gen_p3.Phi());
      h_eff_pT_num->Fill(gen_p3.Pt());
      h_eff_eta_phi_num->Fill(gen_p3.Eta(), gen_p3.Phi());
      h_eff_px_py_den->Fill(gen_p3.X(), gen_p3.Y());
    }
    
    // Clear hits from all superstrips
    w_eraseHits.Start(false);
    for (unsigned int j=0; j<STUB_layer->size(); ++j)
    {
      int superStripCoordinates=superStripCoordinate(STUB_layer->at(j), STUB_ladder->at(j), STUB_module->at(j), STUB_seg->at(j), STUB_strip->at(j));
      std::unordered_map<int, SuperStrip*>::iterator itr=superStripMap.find(superStripCoordinates);
      if (itr!=superStripMap.end()) itr->second->setStatus(0);
    }
    w_eraseHits.Stop();
    
    if (i%10==0) std::cout<<i<<" of "<<nEvents<<" events done."<<std::endl;
    
  } // Event loop
  w_Event.Stop();
  
  std::cout<<"Pattern recognition done of 10000 Events in CPU time = "<<w_Event.CpuTime()<<", real time = "<<w_Event.RealTime()<<std::endl;
  std::cout<<"w_loadHits CPU time = "<<w_loadHits.CpuTime()<<", real time = "<<w_loadHits.RealTime()<<std::endl;
  std::cout<<"w_litPatterns CPU time = "<<w_litPatterns.CpuTime()<<", real time = "<<w_litPatterns.RealTime()<<std::endl;
  std::cout<<"w_eraseHits CPU time = "<<w_eraseHits.CpuTime()<<", real time = "<<w_eraseHits.RealTime()<<std::endl;
  
  // Write efficiency plots to an output file
  TFile *output=new TFile("EfficiencyPlots.root", "recreate");
  h_eff_eta_num->Write();
  h_eff_eta_den->Write();
  h_eff_phi_num->Write();
  h_eff_phi_den->Write();
  h_eff_pT_num->Write();
  h_eff_pT_den->Write();
  h_eff_eta_phi_num->Write();
  h_eff_eta_phi_den->Write();
  h_eff_px_py_num->Write();
  h_eff_px_py_den->Write();
  output->Write();
  output->Close();
  
  return 0;
}
    
