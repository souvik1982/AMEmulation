/**************************************************************
 * Associative Memory Emulation Software for L1 Track Trigger *
 * Pattern Bank Generation Module                             *
 * Authors: Souvik Das                                        *
 *************************************************************/

#include "SuperStrip.h"

SuperStrip::SuperStrip(int id)
{
  id_=id;
  status_=0;
}

int SuperStrip::getStatus()
{
  return status_;
}

void SuperStrip::setStatus(int status)
{
  if (status>0) status_=1; 
  else status_=0;
}

int SuperStrip::getID()
{
  return id_;
}
