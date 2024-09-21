// -*- c++ -*-
#ifndef ANP_RECOBJ_H
#define ANP_RECOBJ_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecObj
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  RecObj is loose reconstructed electron or muon used for fake estimates
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"
#include "PhysicsAnpData/RecMuon.h"
#include "PhysicsAnpData/RecElec.h"
#include "PhysicsAnpData/Ptr.h"

namespace Anp
{
  class RecObj: public ObjectBase, public FourMom
  {    
  public:

    explicit RecObj(const Ptr<RecMuon> &muon); 
    explicit RecObj(const Ptr<RecElec> &elec);
    virtual ~RecObj() {}
    
    static std::string GetObjectType() { return "RecObj"; }

    bool IsElec() const { return fElec.valid(); }
    bool IsMuon() const { return fMuon.valid(); }
    
    int GetFlavor() const;
    int GetCharge() const;

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
    RecObj();

  private:
   
    Ptr<RecElec>      fElec;
    Ptr<RecMuon>      fMuon;
  };

}

#endif
