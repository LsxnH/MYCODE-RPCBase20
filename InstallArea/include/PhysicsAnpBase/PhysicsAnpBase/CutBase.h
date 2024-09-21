// -*- c++ -*-
#ifndef ANP_CUTBASE_H
#define ANP_CUTBASE_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : CutBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Helper class to configure and book CutFlows.
 *           This is common base class for analysis algorithms that 
 *           need to apply cuts.
 * 
 **********************************************************************************/

// C/C++
#include <string>
#include <vector>

// Data
#include "PhysicsAnpData/Ptr.h"

// Base
#include "PhysicsAnpBase/Registry.h"
#include "PhysicsAnpBase/CutFlow.h"

class TDirectory;

namespace Anp
{
  class CutBase
  {
  public:

    CutBase() {}
    virtual ~CutBase() {}

    Ptr<CutFlow> BookCut(const std::string &key, const Registry &reg);

    void PrintCutConfig(std::ostream &os = std::cout) const;

    void PrintCutFlows(std::ostream &os = std::cout) const;

    void SaveCutFlows(TDirectory *dir);

    bool HasNonTrivialCuts();

    void SetCutBaseName(const std::string &name) { fCutBaseName = name; }

  private:

    struct CutData
    {
      Ptr<CutFlow> cut;
    };

    typedef std::vector<CutData> CutVec;

  private:

    bool        fIgnoreTrivialCutFlows;
    std::string fCutBaseName;

    CutVec      fCuts;
  };
}

#endif
