// -*- c++ -*-
#ifndef ANP_VAREXPR_H
#define ANP_VAREXPR_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : VarExpr
 * @Author : Rustem Ospanov
 *
 * @Brief  : Class to evaluate variable expressions using TFormula
 * 
 *  https://root.cern.ch/doc/master/classTFormula.html
 *
 *    - read formula definition and configure TFormula
 *    - set variable values and evaluate the formula expression
 * 
 **********************************************************************************/

// C/C++
#include <cmath>
#include <iostream>
#include <string>

// ROOT
#include "TFormula.h"

// Local
#include "PhysicsAnpData/VarHolder.h"

class TH1;

namespace Anp
{
  class VarExpr
  {
  public:

    VarExpr();
    ~VarExpr();
 
    bool InitExpr(const std::string &name, const std::string &conf, const std::string &hist="");

    bool EvalExpr(const VarHolder &vars, double &value);

    const std::string& GetName   () const { return fExprName;    }
    const std::string& GetConf   () const { return fExprConf;    }
    const std::string& GetFormula() const { return fExprFormula; }    
    const std::string& GetHist   () const { return fExprHist;    }

    unsigned           GetNameVar() const { return fExprNameVar; }
    unsigned           GetHistVar() const { return fExprHistVar; }
    
    unsigned GetCount() const { return fCount; }
    
    void SetDebug(bool flag) { fDebug = flag; }

    void PrintExpr(std::ostream &os = std::cout, const std::string &pad = "") const;

    std::string ExprAsString(const std::string &pad) const;
    
  public:  

    bool InitOnFirstUse();
    
    void ReadParameters(const std::string &config);

    bool ReadValues(const VarHolder &vars);    

  private:

    struct Par
    {
      Par();
      
      bool ReadParValue(const VarHolder &vars)
      {
	return (is_valid = vars.GetVar(par_key, par_value));
      }

      bool          is_valid;
      bool          is_1stvar;
      
      uint32_t      par_key;
      double        par_value;
      
      int           par_number;
      std::string   par_name;
    };

    typedef std::vector<Par> ParVec;
    
  private:    
    
    //
    // Configuration
    //
    std::string   fExprName;
    std::string   fExprConf;
    std::string   fExprFormula;
    std::string   fExprHist;

    unsigned      fExprNameVar;
    unsigned      fExprHistVar;

    bool          fDebug;
    bool          fDoInit;
    
    unsigned      fCount;

    TFormula     *fFormula;

    ParVec        fPars;
  };

  //==============================================================================
  // Inlined functions
  //
  inline bool VarExpr::EvalExpr(const VarHolder &vars, double &value)
  {
    if(fDoInit) {
      fDoInit = false;
      InitOnFirstUse();
    }
    
    if(fPars.empty()) {
      return false;
    }
   
    if(!ReadValues(vars)) {
      return false;
    }

    if(!fFormula) {
      return false;
    }
    
    value = fFormula->Eval(fPars.front().par_value);

    if(std::isnan(value) || fDebug) {
      std::cout << "VarExpr::EvalExpr - " << fExprName << std::endl
		<< "   CLINGP:     " << fFormula->GetExpFormula("CLINGP") << std::endl
		<< "   x[0]:       " << fPars.front().par_value           << std::endl
		<< "   Eval(x[0]): " << value                             << std::endl;

      if(std::isnan(value)) {
	throw std::runtime_error("VarExpr::EvalExpr - expression evaluates to nan");
      }
    }
    
    ++fCount;

    return true;
  }
  
  //==============================================================================
  inline bool VarExpr::ReadValues(const VarHolder &vars)
  {
    for(Par &p: fPars) {
      if(!p.ReadParValue(vars)) {
	return false;
      }
      
      if(fFormula && !p.is_1stvar) {
	fFormula->SetParameter(p.par_number, p.par_value);
      }
    }

    if(fDebug) {
      PrintExpr(std::cout, "   ");
    }
    
    return true;
  }
}

#endif
