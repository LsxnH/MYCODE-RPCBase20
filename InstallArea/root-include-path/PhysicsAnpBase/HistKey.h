// -*- c++ -*-
#ifndef ANP_HISTKEY_H
#define ANP_HISTKEY_H

/**********************************************************************************
 * @Package: PhysicsLight
 * @Class  : HistKey
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  HistKey makes and fills histograms using var keys
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <stdint.h>

// ROOT
#include "TH1.h"
#include "TH2.h"

// Data
#include "PhysicsAnpData/VarHolder.h"

// Base
#include "PhysicsAnpBase/VarCond.h"
#include "PhysicsAnpBase/VarExpr.h"
#include "PhysicsAnpBase/HistMan.h"

class TDirectory;

namespace Anp
{
  class HistKey
  {
  public:

    HistKey();
    ~HistKey();

    void SetHistKey   (const std::string &v) { fHistKey    = v; }
    void SetHistDir   (const std::string &v) { fHistDir    = v; }
    void SetHistPrefix(const std::string &v) { fHistPrefix = v; }

    void ConfigHist(const std::string &key, const Registry &reg);

    void SetDirHist(TDirectory *dir);

    TH2* BookHist2d(std::string hname, uint32_t keyX, uint32_t keyY);

    void FillHist(uint32_t key, double val, double weight);

    void FillHists(const VarHolder &vars,  double weight);

    void FillHist2d(const VarHolder &vars,  double weight);

    bool FillHist2d(HistVars2d &hist, const VarHolder &vars, double weight) const;
    
  private:

    typedef std::vector<Ptr<VarExpr> > ExprVec;
    typedef std::vector<Ptr<VarCond> > CondVec;
    typedef std::vector<HistVars2d>    TH2Vec;

    typedef std::map<uint32_t, std::vector<TH1 *> > TH1Map;

  private:

    void FillTH1(TH1 *h, double val, double weight);

    std::vector<TH1 *>& FindHists(uint32_t key);

    std::vector<TH1 *>& MakeHists(uint32_t key);

    void Init2d();

  private:

    bool                         fDebug;
    bool                         fPrint;
    bool                         fSetSumw2;
    bool                         fAllVis;

    std::string                  fHistDir;
    std::string                  fHistKey;
    std::string                  fHistPrefix;

    bool                         fInit2d; 

    TH1Map                       fTH1Map;       // 1d histogram map
    TH2Vec                       fTH2Vec;       // 2d histogram vector

    ExprVec                      fExprVec;
    CondVec                      fCondVec;
  };

  //-----------------------------------------------------------------------------
  //
  // Inlined functions
  //
  //-----------------------------------------------------------------------------
  inline std::vector<TH1 *>& HistKey::FindHists(uint32_t key) 
  {
    //
    // Find histogram by key, create new if the key does not exist
    //
    TH1Map::iterator hit = fTH1Map.find(key);
    if(hit != fTH1Map.end()) {
      return hit->second;
    }

    if(fDebug) {
      std::cout << "HistKey::FindHists - make hist with key: " << key << std::endl;
    }

    return MakeHists(key);
  }

  //-----------------------------------------------------------------------------
  inline void HistKey::FillTH1(TH1 *h, double val, double weight) 
  {
    //
    // Read new event entry
    //
    if(!h) {
      return;
    }

    if(fDebug) {
      std::cout << "HistKey::FillTH1 - fill hist..." << std::endl;
    }

    if(fAllVis) {
      const double xmax = h->GetXaxis()->GetXmax();
      const double xmin = h->GetXaxis()->GetXmin();
      
      if(xmin < val && val < xmax) {
	h->Fill(val, weight);
      }
      else if (!(val < xmax)) {
	h->Fill(h->GetXaxis()->GetBinCenter(h->GetNbinsX()), weight);
      }
      else if (!(val > xmin)) {
	h->Fill(h->GetXaxis()->GetBinCenter(1), weight);
      }
      else {
	std::cout << "HistKey::FillHists - logic ERROR: " << fHistKey << std::endl;
      }
    }
    else {
      h->Fill(val, weight);
    }
  }

  //-----------------------------------------------------------------------------
  inline void HistKey::FillHist(uint32_t key, double val, double weight) 
  {
    //
    // Read new event entry
    //
    for(TH1 *h: FindHists(key)) {
      FillTH1(h, val, weight);
    }
  }

  //-----------------------------------------------------------------------------
  inline void HistKey::FillHists(const VarHolder &vars, double weight)
  { 
    //
    // Fill 1d histograms
    //
    for(const VarEntry &var: vars.GetVars()) {
      if(fDebug) {
	std::cout << "HistKey::FillHists - key: " << var.GetKey() << std::endl;
      }
      
      FillHist(var.GetKey(), var.GetData(), weight);
    }

    //
    // Process var expressions
    //
    for(Ptr<VarExpr> &ptr: fExprVec) {
      double value = 0.0;
      
      if(ptr->EvalExpr(vars, value)) {
	FillHist(ptr->GetHistVar(), value, weight);
      }
    }

    for(Ptr<VarCond> &ptr: fCondVec) {
      double value = 0.0;
      
      if(ptr->EvalCond(vars, value)) {
	FillHist(ptr->GetHistVar(), value, weight);
      }
    }
  }
}

#endif
