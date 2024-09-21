// -*- c++ -*-
#ifndef ANP_VARCOND_H
#define ANP_VARCOND_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : VarCond
 * @Author : Rustem Ospanov
 *
 * @Brief  : Class to evaluate variable expressions using TFormula
 * 
 *  https://root.cern.ch/doc/master/classTFormula.html
 *
 *    - evaluate expression only of condition evaluates to true
 *    - useful class for filling histograms with cuts applied
 * 
 **********************************************************************************/

// C/C++
#include <cmath>
#include <iostream>
#include <string>

// ROOT
#include "TFormula.h"

// Data
#include "PhysicsAnpData/VarHolder.h"
#include "PhysicsAnpData/Ptr.h"

// Base
#include "PhysicsAnpBase/VarExpr.h"

namespace Anp
{
  class VarCond
  {
  public:

    VarCond();
    ~VarCond() {}
 
    bool InitCond(const std::string &name, 
		  const std::string &cond, 
		  const std::string &formula, 
		  const std::string &hist="");

    bool EvalCond(const VarHolder &vars, double &value);

    const std::string& GetName   () const { return fName;    }
    const std::string& GetCond   () const { return fCond;    }
    const std::string& GetFormula() const { return fFormula; }
    const std::string& GetHist   () const { return fHist;    }

    unsigned           GetNameVar() const { return fNameVar; }
    unsigned           GetHistVar() const { return fHistVar; }
    
    unsigned GetCount() const { return fCount; }
    
    void SetDebug(bool flag) { fDebug = flag; }

    void PrintCond(std::ostream &os = std::cout, const std::string &pad = "") const;

    std::string CondAsString(const std::string &pad) const;
    
  private:    
    
    //
    // Configuration
    //
    std::string   fName;
    std::string   fCond;
    std::string   fFormula;
    std::string   fHist;

    unsigned      fNameVar;
    unsigned      fHistVar;

    bool          fDebug;
    unsigned      fCount;

    Ptr<VarExpr>  fVarCond;
    Ptr<VarExpr>  fVarExpr;
  };

  //==============================================================================
  // Inlined functions
  //
  inline bool VarCond::EvalCond(const VarHolder &vars, double &value)
  {
    if(!fVarCond.valid() || !fVarExpr.valid()) {
      return false;
    }

    double cond = 0.0;
    
    if(!fVarCond->EvalExpr(vars, cond) || !(cond > 0.0)) {
      return false;
    }

    if(!fVarExpr->EvalExpr(vars, value)) {
      return false;
    }
    
    ++fCount;

    return true;
  }
}

#endif
