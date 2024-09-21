// -*- c++ -*-
#ifndef ANP_CUTDR_H
#define ANP_CUTDR_H

/**********************************************************************************
 * @Package: PhysicsAnpLight
 * @Class  : CutDR
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  CutDR is a helper class that implements DR cut with main goal
 *  to optimize performance of overlap removal cuts in PrepCand.
 * 
 **********************************************************************************/

// C/C++
#include <string>

// ROOT
#include "TH1.h"

// Base
#include "PhysicsAnpBase/HistBase.h"
#include "PhysicsAnpBase/Registry.h"

namespace Anp
{
  class CutDR
  {
  public:

    CutDR(const std::string &name, const Registry &reg);
    ~CutDR() {}

    void SetHistDR(TH1 *hall, TH1 *hpass);

    bool IsValid() const { return fValid; }
    bool IsDebug() const { return fDebug; }

    bool PassCutDR(bool good, double dr);

    double GetMinDR() const { return fMinDR; }

    void SaveDRHists(HistBase &base);

    const std::string& GetName() const { return fName; }

    TH1* GetHistAll () const { return hDRAll;  }
    TH1* GetHistPass() const { return hDRPass; }

  private:

    //
    // Default constructor is private and not implemented by design
    //
    CutDR();

  private:
    
    std::string                  fName;
    bool                         fDebug;
    bool                         fValid;

    double                       fMinDR;

    TH1                         *hDRAll;
    TH1                         *hDRPass;
  };

  //
  // Global function to create and configure CutDR objects
  //
  Ptr<CutDR> MakeCutDR(HistBase &base, const std::string &key, const Registry &reg, bool save_hist);

  //==============================================================================
  inline CutDR::CutDR(const std::string &name, const Registry &reg):
    fName  (name),
    fDebug (false), 
    fValid (false),
    fMinDR (-1.0), 
    hDRAll (0),
    hDRPass(0)
    {
      fValid = reg.Get(name, fMinDR);

      reg.Get(     "Debug", fDebug);
      reg.Get(name+"Debug", fDebug);
    }

  //==============================================================================
  inline void CutDR::SetHistDR(TH1 *hall, TH1 *hpass) 
  {
    hDRAll  = hall;
    hDRPass = hpass;
  }

  //==============================================================================
  inline bool CutDR::PassCutDR(bool good, double dr) 
  {
    if(!good) {
      std::cout << "CutDR::PassCutDR - logic error: not good" << std::endl;
      return true;
    }

    if(!fValid) {
      if(fDebug) {
	std::cout << "CutDR::PassCutDR - logic error: not valid" << std::endl;
      }
      return true;
    }

    const bool pass = dr > fMinDR;

    if(hDRAll) {
      hDRAll->Fill(dr);
    }
    if(hDRPass && pass) {
      hDRPass->Fill(dr);
    }

    if(fDebug) {

      std::cout << fName << "::PassCutDR - pass=" << pass << " mindr=" << fMinDR << " dr=" << dr << std::endl;
    }
    
    return pass;
  }

  //==============================================================================
  inline void CutDR::SaveDRHists(HistBase &base)
  {
    if(hDRAll && hDRAll->Integral() > 0.0 && hDRPass) {
      base.ManageHist(hDRAll ); 
      base.ManageHist(hDRPass); 
    }
  }

  //==============================================================================
  inline Ptr<CutDR> MakeCutDR(HistBase &, const std::string &key, const Registry &reg, bool save_hist)
  {
    //
    // Configure 
    //
    Ptr<CutDR> ptr = ObjectFactory<CutDR>::Instance().CreateObject(Anp::CutDR(key, reg));
    
    if(save_hist) {
      TH1 *h0 = new TH1F((key + "_DRAll" ).c_str(), "DR", 100, -0.5, 4.5);
      TH1 *h1 = new TH1F((key + "_DRPass").c_str(), "DR", 100, -0.5, 4.5);
      
      h0->SetDirectory(0);
      h1->SetDirectory(0);

      h0->GetXaxis()->SetTitle((key+" all" ).c_str());
      h1->GetXaxis()->SetTitle((key+" pass").c_str());
      
      ptr->SetHistDR(h0, h1);
      
      if(ptr->IsDebug()) {
	std::cout << "GetCutDR - h0=" << h0 << " h1=" << h1 << std::endl;
      }
    }
    
    if(ptr->IsDebug()) {
      std::cout << "GetCutDR - " << key << ": DR=" << ptr->GetMinDR() << std::endl;
    }
    
    return ptr;    
  }
}

#endif
