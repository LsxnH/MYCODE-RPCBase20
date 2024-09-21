// -*- c++ -*-
#ifndef ANP_ALGBASE_H
#define ANP_ALGBASE_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : AlgBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Helper class to configure and book CutFlows.
 *           This is common base class for analysis algorithms that 
 *           need to apply cuts.
 * 
 **********************************************************************************/

// Base
#include "PhysicsAnpBase/Registry.h"
#include "PhysicsAnpBase/CutBase.h"
#include "PhysicsAnpBase/HistBase.h"

class TDirectory;

namespace Anp
{
  class AlgBase: public virtual HistBase, public virtual CutBase
  {
  public:

    AlgBase():fDirAlgBase(0) {}
    virtual ~AlgBase() {}

  protected:

    void ConfigAlgBase(const Registry &reg, const std::string &name="");

    void SaveAlgBase();

    void SaveAlgBase(TDirectory *dir);

    void SetDirectory(TDirectory *dir) { fDirAlgBase = dir; }
    
  private:

    TDirectory    *fDirAlgBase;
  };
}

#endif
