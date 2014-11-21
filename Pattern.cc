/**************************************************************
 * Associative Memory Emulation Software for L1 Track Trigger *
 * Pattern Bank Generation Module                             *
 * Authors: Souvik Das                                        *
 *************************************************************/

#include <iostream>

#include "Pattern.h"

Pattern::Pattern(unsigned int maxSuperStrips)
{
  status_=0;
  maxSuperStrips_=maxSuperStrips;
  superStripIDs_.resize(maxSuperStrips_, 0);
}

int Pattern::addSuperStrip(int layer, int superStripID)
{
  int ret=0;
  if ((unsigned int)layer<maxSuperStrips_)
  {
    if (superStripIDs_.at(layer)==0) superStripIDs_.at(layer)=superStripID;
    // else std::cout<<"WARNING: Same AM layer "<<layer<<" hit twice. First superstrip ID stored."<<std::endl;
  }
  else
  {
    ret=0;
    std::cout<<"ERROR: Non existent layer "<<layer<<" requested in Pattern"<<std::endl;
  }
  return ret;
}
/*
int Pattern::addSuperStrip(int superStripID)
{
  int ret=0;
  if (superStripIDs_.size()<maxSuperStrips_)
  {
    superStripIDs_.push_back(superStripID);
    ret=1;
  }
  else 
  {
    std::cout<<"WARNING: superStripIDs_.size()>maxSuperStrips_"<<std::endl;
    ret=0;
  }
  return ret;
}
*/
PatternPrimitive* Pattern::getPatternPrimitive()
{
  return &superStripIDs_;
}
  
void Pattern::printSuperStrips()
{
  std::cout<<"Pattern:  ";
  for (unsigned int i=0; i<superStripIDs_.size(); ++i)
  {
    std::cout<<superStripIDs_.at(i)<<" ";
  }
  std::cout<<std::endl;
}
