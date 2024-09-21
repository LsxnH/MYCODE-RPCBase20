// This is -*- c++ -*-
#ifndef ANP_CANDEVENT_H
#define ANP_CANDEVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : CandEvent
 * @Author : Josh Kunkle, Rustem Ospanov
 *
 * @Brief  :
 * 
 *  CandEvent is candidate analysis object created by algorithms.
 *  This code is built on Candidate class in PhysicsAlgs.
 *  
 * 
 **********************************************************************************/

// Data
#include "PhysicsAnpData/VarHolder.h"

// Local
#include "RecoEvent.h"

namespace Anp 
{  
  //==========================================================================================
  namespace Type
  {
    enum Object
    {
      kNone, kLooseLep, kTightLep, kVetoLep, kBJet, kLJet, kSignal, kSFOSPair
    };

    std::string GetObjectAsStr(Object o);

    Object GetTypeFromStr(const std::string &t);
  }

  //==========================================================================================
  class CandEvent: public VarHolder
  {
  public:

    explicit CandEvent(const std::string &name);
    CandEvent(const RecoEvent &event, const std::string &name);

    virtual ~CandEvent();

    static std::string GetObjectType() { return "CandEvent"; }

    void SetWeight(double w) { fReco.SetWeight(w); }

    const std::string& GetCandName() const { return fCandName;         }
    uint32_t           GetCandId  () const { return fCandId;           }
    uint64_t           GetBarcode () const { return fBarcode;          }
    double             GetWeight  () const { return fReco.GetWeight(); }

    Ptr<EventInfo> GetInfo() const { return fReco.GetInfo(); }

    //---------------------------------------------------------------------------------
    // Get RecoEvent from which this candidate was built and RecoEvent with only passed objects
    //    
          RecoEvent& GetReco()       { return fReco; }
    const RecoEvent& GetReco() const { return fReco; }

          RecoEvent& GetPass()       { return fPass; }
    const RecoEvent& GetPass() const { return fPass; }

    //---------------------------------------------------------------------------------
    // Add/Get reconstructed objects selected by candidate
    //
    template<class T> const std::vector<Ptr<T> >& GetVec() const;
    template<class T>       std::vector<Ptr<T> >& GetVec();

    template<class T> const std::vector<Ptr<T> >& GetVec(Type::Object otype) const;
    template<class T>       std::vector<Ptr<T> >& GetVec(Type::Object otype);

    template<class T> bool HasVecType(Type::Object otype) const;

    //---------------------------------------------------------------------------------
    // Record and retrieve pointer to any object that inherits from ObjectBase
    //    
    template<class T> bool AddObjPtr(const std::string &key, const Ptr<T> &ptr);

    template<class T> Ptr<T> GetObjPtr(const std::string &key) const;

    const std::map<std::string, Ptr<ObjectBase> >& GetObjMap() const { return fMapObj; }

    //---------------------------------------------------------------------------------
    // Helper functions
    //
    Ptr<CandEvent> MakeCopy(const std::string &name) const;

    void ClearEvent();

    void SwitchToLocalReco();

    void Print(std::ostream &os = std::cout, const std::string &pad = "") const;

  private:

    template<class T> void SwitchToLocal(std::vector<Ptr<T> > &vec);

    template<class T> void SwitchToLocalDerived(std::vector<Ptr<T> > &vec);

    friend class RecoEvent;

  private:

    typedef std::map<Type::Object,  std::vector<Ptr<RecElec> > > ObjMapElec;
    typedef std::map<Type::Object,  std::vector<Ptr<RecMuon> > > ObjMapMuon;
    typedef std::map<Type::Object,  std::vector<Ptr<Lepton > > > ObjMapLep;
    typedef std::map<Type::Object,  std::vector<Ptr<RecJet > > > ObjMapJet;
    typedef std::map<Type::Object,  std::vector<Ptr<RecTau > > > ObjMapTau;

    typedef std::map<std::string, Ptr<ObjectBase> > ObjMap;

    //
    // Default constructor is private and not implemented by design
    //
    CandEvent();

  private:
    
    std::string               fCandName;     // Candidate name
    uint32_t                  fCandId;       // Candidate name hash id
    uint64_t                  fBarcode;      // Candidate unique barcode

    RecoEvent                 fReco;         // Local copy of RecoEvent with all      objects and with EventInfo
    RecoEvent                 fPass;         // Local copy of RecoEvent with selected objects and w/o  EventInfo

    //
    // These objects below are created for this specific candidate
    //
    std::vector<Ptr<RecObj> > fObj;
    std::vector<Ptr<Lepton> > fLepton;

    ObjMapElec                fMapElec;
    ObjMapMuon                fMapMuon;
    ObjMapLep                 fMapLep;
    ObjMapJet                 fMapJet;
    ObjMapTau                 fMapTau;

    ObjMap                    fMapObj;
  };
}

//==========================================================================================
// Include template and inlined code
//==========================================================================================
#include "CandEvent.icc"

#endif
