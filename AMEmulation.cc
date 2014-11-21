#include <TROOT.h>
#include <TStyle.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <map>
#include <TStopwatch.h>

#include <TRandom3.h>

TRandom3 *r3=new TRandom3();

void fillRandomAddresses(std::vector<int> &hitAddresses)
{
  // std::cout<<" Hits fed: ";
  for (unsigned int i=0; i<6; ++i)
  {
    int rand=int(r3->Rndm()*double(0xffff));
    // std::cout<<rand<<" ";
    hitAddresses.push_back(rand);
  }
  // std::cout<<std::endl;
}  

class SuperStrip
{
  public:
    int address_;
    int status_;
    
  public:
    SuperStrip(int address)
    {
      address_=address;
      status_=0;
    }
  
    void setStatus(int status)
    {
      status_=status;
    }
};

class Pattern
{
  public:
    std::vector<SuperStrip*> superStrips_;
    int status_;
    
  public:
    Pattern()
    {
      status_=0;
    }
  
    void addSuperStrip(int superStripAddress)
    {
      SuperStrip *superStrip=new SuperStrip(superStripAddress);
      superStrips_.push_back(superStrip);
    }
  
    int getStatus()
    {
      unsigned int s=superStrips_.size(), t=0;
      for (unsigned int i=0; i<s; ++i)
      {
        t+=superStrips_.at(i)->status_;
      }
      if (t>=(s-1)) status_=1;
      return status_;
    }
    
    void printSuperStrips()
    {
      std::cout<<"Pattern:  ";
      for (unsigned int i=0; i<superStrips_.size(); ++i)
      {
        std::cout<<superStrips_.at(i)->address_<<" ";
      }
      std::cout<<std::endl;
    }
};

void AMEmulation()
{

  TStopwatch stopWatch;
  
  unsigned int nTrainingEvents=1e3;
  unsigned int nTestingEvents=1e5;

  std::vector<std::vector<int> > hitBank;
  
  // Bank Generation
  std::vector<Pattern*> patternBank;
  stopWatch.Start();
  for (unsigned int event=0; event<nTrainingEvents; ++event)
  {
    std::vector<int> hitAddresses; // Say, 16 bit numbers
    fillRandomAddresses(hitAddresses);
    hitBank.push_back(hitAddresses);
  
    std::map<int, SuperStrip*> superStripMap; // Where int = 12 bit address of superstrip
    for (unsigned int i=0; i<hitAddresses.size(); ++i)
    {
      int superStripAddress=hitAddresses.at(i); // >>4;  // Will depend on geometry and granularity of the detector
      SuperStrip *superStrip=new SuperStrip(superStripAddress);
      superStripMap.insert(std::pair<int, SuperStrip*>(superStripAddress, superStrip));
    }
    Pattern *pattern=new Pattern();
    for (std::map<int, SuperStrip*>::iterator superStripMap_i=superStripMap.begin(); superStripMap_i!=superStripMap.end(); ++superStripMap_i)
    {
      pattern->addSuperStrip(superStripMap_i->first);
    }
    patternBank.push_back(pattern);
  }
  stopWatch.Stop();
  
  std::cout<<"Bank generation of "<<nTrainingEvents<<" Patterns done in CPU time = "<<stopWatch.CpuTime()<<", real time = "<<stopWatch.RealTime()<<std::endl;
  
  // Pattern Matching
  TStopwatch w_getHits, w_newMap, w_loadHits, w_litPatterns, w_OffMap;
  stopWatch.Start();
  w_newMap.Start(false);
  std::map<int, SuperStrip*> superStripMap_new;
  for (unsigned int i=0; i<patternBank.size(); ++i)
  {                                                                                                                                                   
    for (unsigned int j=0; j<patternBank.at(i)->superStrips_.size(); ++j)                                                                             
    {                                                                                                                                                 
      superStripMap_new.insert(std::pair<int, SuperStrip*>(patternBank.at(i)->superStrips_.at(j)->address_, patternBank.at(i)->superStrips_.at(j)));  
    }                                                                                                                                                 
  }
  w_newMap.Stop();                                                                                                                                                 
  for (unsigned int event=0; event<nTestingEvents; ++event)
  {
    // std::cout<<"Event "<<event<<std::endl;
    w_getHits.Start(false);
    std::vector<int> hitAddresses; // Say, 16 bit numbers
    int randEvent=int(r3->Rndm()*nTrainingEvents);
    hitAddresses=(hitBank.at(randEvent));
    w_getHits.Stop();
    
    w_loadHits.Start(false);
    for (unsigned int i=0; i<hitAddresses.size(); ++i)
    {
      int superStripAddress=hitAddresses.at(i); //>>4;  // Will depend on geometry and granularity of the detector
      superStripMap_new[superStripAddress]->setStatus(1);
    }
    w_loadHits.Stop();
    
    w_litPatterns.Start(false);
    for (unsigned int i=0; i<patternBank.size(); ++i)
    {
      if (patternBank.at(i)->getStatus()==1) 
      {
        // patternBank.at(i)->printSuperStrips();
        patternBank.at(i)->status_=0;
      }
    }
    w_litPatterns.Stop();
    
    w_OffMap.Start(false);
    for (unsigned int i=0; i<hitAddresses.size(); ++i)
    {
      int superStripAddress=hitAddresses.at(i); //>>4;  // Will depend on geometry and granularity of the detector
      superStripMap_new[superStripAddress]->setStatus(0);
    }
    w_OffMap.Stop();
    
  }
  stopWatch.Stop();
  std::cout<<"Pattern recognition done of "<<nTestingEvents<<" Events in CPU time = "<<stopWatch.CpuTime()<<", real time = "<<stopWatch.RealTime()<<std::endl;
  std::cout<<"w_getHits CPU time = "<<w_getHits.CpuTime()<<", real time = "<<w_getHits.RealTime()<<std::endl;
  std::cout<<"w_newMap CPU time = "<<w_newMap.CpuTime()<<", real time = "<<w_newMap.RealTime()<<std::endl;
  std::cout<<"w_loadHits CPU time = "<<w_loadHits.CpuTime()<<", real time = "<<w_loadHits.RealTime()<<std::endl;
  std::cout<<"w_litPatterns CPU time = "<<w_litPatterns.CpuTime()<<", real time = "<<w_litPatterns.RealTime()<<std::endl;
  std::cout<<"w_OffMap CPU time = "<<w_OffMap.CpuTime()<<", real time = "<<w_OffMap.RealTime()<<std::endl;
  
  
}    
    
    
    
    
    
    
    
    
     
