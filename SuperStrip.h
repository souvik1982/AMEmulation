/**************************************************************
 * Associative Memory Emulation Software for L1 Track Trigger *
 * Pattern Bank Generation Module                             *
 * Authors: Souvik Das                                        *
 *************************************************************/

class SuperStrip
{
  private:
    int id_;
    int status_;
    
  public:
    SuperStrip(int id);
    int getStatus();
    void setStatus(int status);
    int getID();
};
