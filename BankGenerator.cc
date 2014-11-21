/**************************************************************
 * Associative Memory Emulation Software for L1 Track Trigger *
 * Pattern Bank Generation Module                             *
 * Authors: Souvik Das                                        *
 *************************************************************/

#include <iostream>
#include <map>

#include <TFile.h>
#include <TChain.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>

#include "Pattern.h"
#include "Coordinator.h"

bool verbose=false;
bool illustrate=true;

bool patternBankSorter (std::pair<PatternPrimitive, unsigned int> a, std::pair<PatternPrimitive, unsigned int> b)
{
  return (a.second > b.second);
}

int main()
{
  // Open a TChain to hold the contents of the ROOT file containing stubs
  TChain *tree = new TChain("L1TrackTrigger");
  tree->Add("/Users/souvik/CMSPhase2Upgrades/Samples/SLHC/GEN/612_SLHC6_MU/MU_612_SLHC6.root");
  
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
  
  // Open a TChain to hold the generator level information
  TChain *MC = new TChain("MC");
  MC->Add("/Users/souvik/CMSPhase2Upgrades/Samples/SLHC/GEN/612_SLHC6_MU/MU_612_SLHC6.root");
  
  // Declare and assign variables for gen particle information
  int gen_n;
  std::vector<float> *gen_px=0;
  std::vector<float> *gen_py=0;
  std::vector<float> *gen_pz=0;
  MC->SetBranchAddress("gen_n", &gen_n);
  MC->SetBranchAddress("gen_px", &gen_px);
  MC->SetBranchAddress("gen_py", &gen_py);
  MC->SetBranchAddress("gen_pz", &gen_pz);
  
  // Histograms for illustration
  TH1F *h_gen_eta=new TH1F("h_gen_eta", "h_gen_eta", 100, -2.5, 2.5);
  TH1F *h_gen_eta_1HitPerLayer=new TH1F("h_gen_eta_1HitPerLayer", "h_gen_eta_1HitPerLayer", 100, -2.5, 2.5);
  TH1F *h_gen_phi=new TH1F("h_gen_phi", "h_gen_phi", 100, -3.14, 3.14);
  TH2F *h_nStubs_eta=new TH2F("h_nStubs_eta", "h_nStubs_eta", 200, -2.5, 2.5, 15, 0., 15.);
  TH2F *h_nStubs_eta_1HitPerLayer=new TH2F("h_nStubs_eta_1HitPerLayer", "h_nStubs_eta_1HitPerLayer", 200, -2.5, 2.5, 15, 0., 15.);
  
  // The map of patterns that will be the Pattern Bank
  std::map<PatternPrimitive, unsigned int> patternBank;
  
  // Event loop
  int nEvents=tree->GetEntries();
  double n1HitInLayer=0;
  for (int i=0; i<nEvents; ++i)
  {
    tree->GetEvent(i);
    MC->GetEvent(i);
    
    // Generator level track information
    TVector3 gen_p3(gen_px->at(0), gen_py->at(0), gen_pz->at(0));
    if (illustrate) h_gen_eta->Fill(gen_p3.Eta());
    if (illustrate) h_gen_phi->Fill(gen_p3.Phi());
    
    if (gen_p3.Pt()>10)
    {
      // 1 stub per layer events are used for pattern generation
      std::vector<int> nHitsLayer(23, 0);
      std::vector<int> hitInLayer(23, 0); 
      bool multipleHitsInLayer=false;
      if (illustrate) h_nStubs_eta->Fill(gen_p3.Eta(), STUB_layer->size());
      for (unsigned int j=0; j<STUB_layer->size(); ++j)
      {
        if (nHitsLayer.at(STUB_layer->at(j))==0)
        {
          hitInLayer.at(STUB_layer->at(j))=superStripCoordinate(STUB_layer->at(j), STUB_ladder->at(j), STUB_module->at(j), STUB_seg->at(j), STUB_strip->at(j));
          ++nHitsLayer.at(STUB_layer->at(j));
        }
        else
        {
          multipleHitsInLayer=true;
          break;
        }
      }
      if (multipleHitsInLayer) continue;
      // if (STUB_layer->size()<6) continue;
    
      // Nice, now these events have 1 hit per layer
      // Create patterns from them.
      if (illustrate) h_nStubs_eta_1HitPerLayer->Fill(gen_p3.Eta(), STUB_layer->size());
      if (illustrate) h_gen_eta_1HitPerLayer->Fill(gen_p3.Eta());
      ++n1HitInLayer;
      Pattern *pattern=new Pattern(layersOnChip); // Maximum number of layers on the 3D chip
      for (unsigned int j=0; j<23; ++j)
      {
        if (nHitsLayer.at(j)>0)
        {
          pattern->addSuperStrip(returnAMChipLayer(j), hitInLayer.at(j));
        }
      }
    
      // pattern->printSuperStrips();
      ++patternBank[*(pattern->getPatternPrimitive())];
    }
    
    // if (i%1000==0) std::cout<<"Processed "<<i<<" events, Pattern Bank is of size "<<patternBank.size()<<std::endl;
    
  } // Event loop
  
  // Sort the patterns in descending order of popularity
  std::vector<std::pair<PatternPrimitive, unsigned int> > patternBank_sorted;
  for (std::map<PatternPrimitive, unsigned int>::iterator itr=patternBank.begin(); itr!=patternBank.end(); ++itr) patternBank_sorted.push_back(*itr);
  std::sort(patternBank_sorted.begin(), patternBank_sorted.end(), patternBankSorter);
  
  // Print out the sorted pattern bank
  // And store it in a ROOT ntuple
  TFile *file=new TFile("PatternBank.root", "recreate");
  TTree *outtree=new TTree("PatternBank", "PatternBank");
  int popularity;
  int nLayersOnAMChip=layersOnChip;
  int t_pattern[layersOnChip];
  outtree->Branch("popularity", &popularity, "popularity/I");
  outtree->Branch("nLayersOnAMChip", &nLayersOnAMChip, "nLayersOnAMChip/I");
  outtree->Branch("pattern", t_pattern, "pattern[nLayersOnAMChip]/I");
  for (unsigned int i=0; i<patternBank_sorted.size(); ++i)
  {
    PatternPrimitive pattern=patternBank_sorted.at(i).first;
    popularity=patternBank_sorted.at(i).second;
    if (verbose) std::cout<<"Popularity = "<<popularity<<": ";
    for (unsigned int j=0; j<(unsigned int)nLayersOnAMChip; ++j)
    {
      t_pattern[j]=pattern.at(j);
      if (verbose) std::cout<<pattern.at(j)<<", ";
    }
    outtree->Fill();
    if (verbose) std::cout<<std::endl;
  }
  // outtree->Print();
  file->cd();
  outtree->Write();
  h_gen_eta->Write();
  h_gen_eta_1HitPerLayer->Write();
  h_gen_phi->Write();
  h_nStubs_eta->Write();
  h_nStubs_eta_1HitPerLayer->Write();
  file->Close();
  
  std::cout<<"nEvents = "<<nEvents<<", n1HitInLayer = "<<n1HitInLayer<<" % = "<<n1HitInLayer/nEvents*100.<<std::endl;
  
  return 0;
}
 
  
