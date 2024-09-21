// -*- c++ -*-
#ifndef ANP_LEPTON_H
#define ANP_LEPTON_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : Lepton
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Generic light lepton class - can be either RecMuon or RecElec
 * 
 **********************************************************************************/

// Data
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"
#include "PhysicsAnpData/RecElec.h"
#include "PhysicsAnpData/RecMuon.h"
#include "PhysicsAnpData/Ptr.h"

namespace Anp
{
  class Lepton: public ObjectBase, public FourMom
  {
  public:

    explicit Lepton(const Ptr<RecMuon> &muon); 
    explicit Lepton(const Ptr<RecElec> &elec); 
    virtual ~Lepton() {}

    static std::string GetObjectType() { return "Lepton"; }

    bool IsElec() const { return fElec.valid(); }
    bool IsMuon() const { return fMuon.valid(); }
    
    int GetFlavor    () const;
    int GetCharge    () const;

    const Ptr<RecElec>& GetElec() const { return fElec; }
    const Ptr<RecMuon>& GetMuon() const { return fMuon; }

    Ptr<RecElec>& GetElec() { return fElec; }
    Ptr<RecMuon>& GetMuon() { return fMuon; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:

    //
    // Default constructor is private and not implemented by design
    //
    Lepton();

  private:    

    Ptr<RecElec>      fElec;
    Ptr<RecMuon>      fMuon;
  };
}

#endif
