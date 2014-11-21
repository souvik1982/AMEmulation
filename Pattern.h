/**************************************************************
 * Associative Memory Emulation Software for L1 Track Trigger *
 * Pattern Bank Generation Module                             *
 * Authors: Souvik Das                                        *
 *************************************************************/
 
#include <stdlib.h>
#include <vector>

#include "SuperStrip.h"

typedef std::vector<int> PatternPrimitive;

class Pattern
{
  private:
    PatternPrimitive superStripIDs_;
    int status_;
    unsigned int maxSuperStrips_;
  
  public:
    Pattern(unsigned int maxSuperStrips=8);
    // int addSuperStrip(int superStripID);
    int addSuperStrip(int layer, int superStripID);
    PatternPrimitive* getPatternPrimitive();
    // int getStatus(int threshold);
    void printSuperStrips();
};
