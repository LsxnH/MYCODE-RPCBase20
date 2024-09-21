// -*- c++ -*-
#ifndef ANP_CUTEXPR_H
#define ANP_CUTEXPR_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : CutExpr
 * @Author : Rustem Ospanov
 *
 * @Brief  : Class to configure and apply cuts using expressions interpreted with TFormula
 * 
 *  CutItem applies a set of cuts:
 *    - read cut definitions from Registry
 *    - read variables and evaluate cut expression
 * 
 **********************************************************************************/

// C/C++
#include <cmath>
#include <iostream>
#include <string>

// Data
#include "PhysicsAnpData/VarHolder.h"
#include "PhysicsAnpData/Ptr.h"

// Local
#include "PhysicsAnpBase/CutItem.h"
#include "PhysicsAnpBase/VarExpr.h"

namespace Anp
{
  class Registry;

  class CutExpr
  {
  public:

    CutExpr();
    ~CutExpr();

    bool InitCut(const Registry &reg);

    template<class T> Cut::State PassCut(const T &event);

    const std::string& GetName() const { return fCutName; }
    const std::string& GetConf() const { return fCutConf; }
    
    bool IsValidExpr() const { return fVarExpr; }

    void PrintConfig(std::ostream &os = std::cout, const std::string &pad = "") const;

    std::string ConfigAsString(const std::string &pad) const;
    
  private:

    std::ostream& log() const;

  private:  
    
    //
    // Configuration
    //
    std::string  fCutName;
    std::string  fCutConf;

    bool         fDebug;
    bool         fDummy;

    VarExpr     *fVarExpr;
  };
  
  //-----------------------------------------------------------------------------
  // Inlined member functions
  //
  template<class T> inline Cut::State CutExpr::PassCut(const T &event)
  {
    if(fDummy) {
      return Cut::Pass;
    }

    if(!fVarExpr) {
      return Cut::None;
    }
    
    double value = 0.0;
    
    if(!fVarExpr->EvalExpr(event, value)) {
      return Cut::None;      
    }
    
    if(value > 0.0) {
      return Cut::Pass;
    }

    return Cut::Fail;
  }
}

#endif
