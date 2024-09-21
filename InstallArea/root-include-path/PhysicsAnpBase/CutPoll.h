// -*- c++ -*-
#ifndef ANP_CUTPOLL_H
#define ANP_CUTPOLL_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : CutPoll
 * @Author : Rustem Ospanov
 *
 * @Brief  : Count events with weight by sample for cut-flows
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <set>

// ROOT
#include "TH1.h"

// Data
#include "PhysicsAnpData/Ptr.h"
#include "PhysicsAnpData/VarEntry.h"

namespace Anp
{
  typedef std::set<unsigned> SampleSet;

  class CutPoll
  {
  public:

    CutPoll();
    ~CutPoll() {}
    
    void CountEvent(double weight);
    
    void Print(const std::string &key) const;
    
    std::string Convert2Str(double val, double err) const;

    bool GetCountError(double &val, double &err, bool use_raw=false) const;

    std::pair<std::string, std::string> GetCountErrorAsPair(bool use_raw = false) const;

    void SetName(const std::string &name) { fName = name; }

    const std::string& GetName() const { return fName; }
    TH1*               GetHist() const { return fHist; }

    void AddPlotVar(const Ptr<VarEntry> &ptr, const std::string &key, const std::string &hist);

  private:

    struct Count 
    {
      Count() :sumn(0), sumw(0.0), sumw2(0.0) {}
      
      unsigned sumn;
      double   sumw;
      double   sumw2;
    };

  private:

    typedef std::map<unsigned, Count> SampleMap;
    
  private:

    std::string    fName;
    std::string    fHistKey;

    Count          fCount;

    TH1           *fHist;
    Ptr<VarEntry>  fVarPtr;
  };

  //
  // Inlined functions
  //
  inline void Anp::CutPoll::CountEvent(double weight)
  {
    fCount.sumn  += 1;
    fCount.sumw  += weight;
    fCount.sumw2 += weight*weight;

    if(fHist && fVarPtr.valid()) {
      fHist->Fill(fVarPtr->GetData(), weight);
    }
  }
}

#endif
