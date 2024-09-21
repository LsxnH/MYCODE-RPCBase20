// -*- c++ -*-
#ifndef ANP_HISTBASE_H
#define ANP_HISTBASE_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : HistBase
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  HistBase is a base class for creating ROOT histograms
 * 
 **********************************************************************************/

// C/C++
#include <string>
#include <vector>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

// Data
#include "PhysicsAnpData/RecoEvent.h"

// Local
#include "PhysicsAnpBase/HistKey.h"

class TDirectory;
class TProfile;

namespace Anp
{
  class AlgEvent;

  class HistBase
  {
  public:

    HistBase();
    virtual ~HistBase();

    void ConfigHistBase(const Registry &reg);

    Ptr<HistKey> BookHist(const Registry &reg, 
			  const std::string &dir="", 
			  const std::string &pref="");

    TH1* GetTH1(const std::string &key, const std::string &name = "");    
    TH2* GetTH2(const std::string &key, const std::string &name = "");
    TH3* GetTH3(const std::string &key, const std::string &name = "");

    TProfile* GetTProfile(const std::string &name, int nbin, double xmin, double xmax);

    TH1* MakeTH1(const std::string &key, const std::string &name = "");
    TH2* MakeTH2(const std::string &key, const std::string &name = "");

    void PostProc(TH1* hist);

    void FillTH1(TH1 *h, double vl,            double weight = 1.0);
    void FillTH2(TH2 *h, double vx, double vy, double weight = 1.0);

    void ManageHist(TH1 *h);

    void SaveHist(TDirectory *dir);

    void ClearHistLists();

    std::vector<Ptr<HistKey> >& GetHists() { return fHists; }

  protected:

    typedef std::vector<TH1 *>           TH1Vec;
    typedef std::map<std::string, TH1 *> HistAll;
    typedef std::vector<Ptr<HistKey> >   HistVec;

  protected:

    bool                         fSetSumw2;     // Set TH1 sumw2 option
    bool                         fPostProc;     // Run post processing
    bool                         fSaveKeyHists; // Do not make key histograms
    bool                         fShowOverflow; // Make 1st and last bins to be overflows
    std::string                  fKeyHist;

    HistVec                      fHists;
    HistAll                      fHistAll;      // All unique TH1 and TH2 histograms
    TH1Vec                       fTH1Vec;       // All TH1 objects managed by this class
  };

  //-----------------------------------------------------------------------------
  // Inlined functions
  //
  inline void HistBase::FillTH1(TH1 *h, double vl, double weight)
  {    
    if(!h) {
      return;
    }

    if(fShowOverflow) {
      const double xmax = h->GetXaxis()->GetXmax();
      const double xmin = h->GetXaxis()->GetXmin();
      
      if(xmin < vl && vl < xmax) {
	h->Fill(vl, weight);
      }
      else if (!(vl < xmax)) {
	h->Fill(h->GetXaxis()->GetBinCenter(h->GetNbinsX()), weight);
      }
      else if (!(vl > xmin)) {
	h->Fill(h->GetXaxis()->GetBinCenter(1), weight);
      }
      else {
	std::cout << "HistBase::FillTH1 - logic ERROR!" << std::endl;
      }
    }
    else {
      h->Fill(vl, weight);
    }
  }

  //-----------------------------------------------------------------------------
  inline void HistBase::FillTH2(TH2 *h, double vx, double vy, double weight)
  {    
    if(!h) {
      return;
    }

    h->Fill(vx, vy, weight);
  }
}

#endif
