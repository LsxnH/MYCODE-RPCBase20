// This is -*- c++ -*-
#ifndef ANP_RECMUON_H
#define ANP_RECMUON_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecMuon
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  This class implements light version of Analysis::Muon class
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class RecMuon: public ObjectBase, public FourMom
  {
  public:

    RecMuon();
    virtual ~RecMuon() {}

    static std::string GetObjectType() { return "RecMuon"; }    

    int  GetCharge() const { return fCharge; }
    void SetCharge(int val) { fCharge = val; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:
    
    int fCharge;
  };
}

#endif
