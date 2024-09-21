#ifndef ANP_RUNALGS_H
#define ANP_RUNALGS_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : RunAlgs
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 * This class creates, configures and executes a list of AlgEvent algorithms.
 * 
 **********************************************************************************/

// C/C++
#include <vector>

// Local
#include "PhysicsAnpBase/AlgEvent.h"

namespace Anp
{
  class RunAlgs: public virtual AlgEvent
  {
  public:
    
    RunAlgs();
    virtual ~RunAlgs();
    
    void Config(const Anp::Registry &reg);
    
    void Signal(const Anp::Registry &reg, const std::string &signal);

    void Save(TDirectory *);

    bool Init();

    void Exec();

    void Done();
    
  protected:

    bool                      fDebug;       // Print some debug info
    bool                      fPrint;       // Print information
    bool                      fDoTimer;     // Measure algorithm execution times
  };
}

#endif
