// -*- c++ -*-
#ifndef ANP_RECOEVENT_H
#define ANP_RECOEVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecoEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  RecoEvent is a data class which holds pointers to objects read from TTree
 *  
 * - Pointers to objects stored using Ptr<T> smart pointer template
 * - Objects are accessed with GetVec<T> functions
 * - Event variables stored with VarHolder base of RecoEvent
 * 
 **********************************************************************************/

// C++
#include <map>
#include <vector>

// Data
#include "PhysicsAnpData/AnyObj.h"
#include "PhysicsAnpData/EventInfo.h"
#include "PhysicsAnpData/Lepton.h"
#include "PhysicsAnpData/Ptr.h"
#include "PhysicsAnpData/RecCluster.h"
#include "PhysicsAnpData/RecElec.h"
#include "PhysicsAnpData/RecObj.h"
#include "PhysicsAnpData/RecJet.h"
#include "PhysicsAnpData/RecTrackJet.h"
#include "PhysicsAnpData/RecMuon.h"
#include "PhysicsAnpData/RecPhoton.h"
#include "PhysicsAnpData/RecTau.h"
#include "PhysicsAnpData/RecTrack.h"
#include "PhysicsAnpData/RecVertex.h"
#include "PhysicsAnpData/TruthJet.h"
#include "PhysicsAnpData/TruthPart.h"
#include "PhysicsAnpData/TruthVtx.h"

namespace Anp 
{  
  class CandEvent;

  class RecoEvent: public VarHolder
  {
  public:

    RecoEvent() {}
    virtual ~RecoEvent() {}

    static std::string GetObjectType() { return "RecoEvent"; }

    //---------------------------------------------------------------------------------
    // Functions to access basic objects 
    //
    template<class T>       std::vector<Ptr<T> >& GetVec();
    template<class T> const std::vector<Ptr<T> >& GetVec() const;

    //---------------------------------------------------------------------------------
    // Functions to access arbitrary data that does not match existing reco/true objects
    //
          std::vector<Ptr<AnyObj> >& GetAnyVec(const std::string &key);
    const std::vector<Ptr<AnyObj> >& GetAnyVec(const std::string &key) const;

    const std::map<std::string, std::vector<Ptr<AnyObj> > >& GetAnyMap() const;
    
    bool HasAnyVec(const std::string &key) const;

    //---------------------------------------------------------------------------------
    // Utility/helper functions
    //
    void ClearEvent();

    void FillCopy(RecoEvent &copy) const;

    void FillRecoCopy(RecoEvent &copy) const;

    Ptr<EventInfo> GetInfo() const { return fInfo; }

    void SetInfo(Ptr<EventInfo> info) { fInfo = info; }

    void SetWeight(double weight);

    double GetWeight() const;

    void Print(std::ostream &os = std::cout, const std::string &pad = "") const;
    
  private:

    friend class CandEvent;

    typedef std::map<std::string, std::vector<Ptr<AnyObj> > > MapAnyObj;

  private:
    
    //
    // These objects below are readout from ntuples
    //
    std::vector<Ptr<RecElec    > >  fElec;
    std::vector<Ptr<RecJet     > >  fJet;
    std::vector<Ptr<RecTrackJet> >  fTrackJet;
    std::vector<Ptr<RecMuon    > >  fMuon;
    std::vector<Ptr<RecPhoton  > >  fPhoton;
    std::vector<Ptr<RecTrack   > >  fTrack;
    std::vector<Ptr<RecTau     > >  fTau;
    std::vector<Ptr<RecCluster > >  fCluster;
    std::vector<Ptr<RecVertex  > >  fVertex;
    std::vector<Ptr<RecHit     > >  fHits;

    std::vector<Ptr<TruthPart  > >  fTruePart;
    std::vector<Ptr<TruthVtx   > >  fTrueVtx;
    std::vector<Ptr<TruthJet   > >  fTrueJet;
    
    Ptr<EventInfo>                  fInfo;

    MapAnyObj                       fMapAny;

    //
    // Candidate events created from this RecoEvent
    //
    std::vector<Ptr<CandEvent> >    fCand;
  };

  //===========================================================================================
  // Global functions and function templates
  //===========================================================================================
  template<class T> void ClearVecData(std::vector<Ptr<T> > &vec);

  template<class T> void CopyVecData(const std::vector<Ptr<T> > &in, std::vector<Ptr<T> > &out);
}

//
// Include template and inlined code
//
#include "RecoEvent.icc"
#endif
