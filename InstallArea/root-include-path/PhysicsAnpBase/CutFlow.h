// -*- c++ -*-
#ifndef ANP_CUTFLOW_H
#define ANP_CUTFLOW_H 

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : CutFlow
 * @Author : Rustem Ospanov
 *
 * @Brief  : Configure and run analysis cut-flow
 * 
 **********************************************************************************/

// C/C++
#include <iostream>
#include <map>

// Data
#include "PhysicsAnpData/Ptr.h"

// Base
#include "PhysicsAnpBase/Registry.h"
#include "PhysicsAnpBase/CutExpr.h"
#include "PhysicsAnpBase/CutItem.h"
#include "PhysicsAnpBase/CutPoll.h"

namespace Anp
{
  class CutFlow
  {
  public:

    CutFlow();
    ~CutFlow();
    
    void ConfCut(const std::string &key, const Registry &reg);

    void SetName(const std::string &name) { fName = name; }

    template<class T> Cut::State PassCut(const T &event, double weight = 1.0);

    const std::string& GetName() const { return fName; }
    const std::string& GetKey () const { return fKey;  }

    void AddVarPtr(const Ptr<VarEntry> &ptr);

    void AddPlotVar(const Ptr<VarEntry> &ptr, const std::string &key, const std::string &hist);

    void PrintCut (std::ostream &os, const std::string &pad="   ") const { PrintCuts(os, pad); }
    void PrintCuts(std::ostream &os, const std::string &pad="   ") const;

    void PrintConf(std::ostream &os, const std::string &pad="") const;

    void SaveCutFlow(const std::string &key, TDirectory *dir) const;

    bool HasCuts() const { return !fCuts.empty(); }

    void SetDebug(bool flag) { fDebug = flag; }

  public:

    struct CutPair
    {
      const std::string& GetCutName() const;
      const std::string& GetCutConf() const;

      CutItem  icut;
      CutExpr  ecut;

      CutPoll  poll;
    };

    typedef std::vector<Ptr<CutPair> > CutVec;

  private:

    void AddCut(const std::string &key, const Registry &reg);

    void SaveSubFlow(const Ptr<CutPoll> &input, 
		     const std::string &key, TDirectory *dir, const CutVec &cuts) const;

    void SaveVarHist(const CutPoll &poll, const std::string &prefix, TDirectory *dir) const;

    std::ostream& log() const;

  private:

    bool                        fDebug;
    std::string                 fName;
    std::string                 fKey;

    CutVec                      fCuts;
    Ptr<CutPoll>                fInput;
  };

  //-----------------------------------------------------------------------------
  // Template implementations and inlined functions
  //-----------------------------------------------------------------------------
  template<class T> inline Cut::State CutFlow::PassCut(const T &event, double weight)
  {
    //
    // Count all events
    //
    if(fInput.valid()) {
      fInput->CountEvent(weight);
    }

    if(fCuts.empty()) {
      return Cut::None;
    }

    //
    // Apply cuts
    //
    bool pass = true;
    
    for(Ptr<CutPair> &cut: fCuts) {
      Cut::State value = Cut::None;

      if(cut->ecut.IsValidExpr()) {
	value = cut->ecut.PassCut(event);
      }
      else {
	value = cut->icut.PassCut(event);
      }

      if(value == Cut::Pass) {
	cut->poll.CountEvent(weight); 
	
	if(fDebug) {
	  std::cout << "CutFlow - name=" << fName << " key=" << fKey << ": passed" << std::endl;
	}
      }
      else {
	if(fDebug) {
	  std::cout << "CutFlow - name=" << fName << " key=" << fKey << ": failed" << std::endl;
	}

	pass = false;
	break;
      }
    }

    if(pass) {
      return Cut::Pass;
    }

    return Cut::Fail;
  }
}

#endif
