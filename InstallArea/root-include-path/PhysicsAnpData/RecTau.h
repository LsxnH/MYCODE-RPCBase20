// -*- c++ -*-
#ifndef ANP_RECTAU_H
#define ANP_RECTAU_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : Lepton
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Generic tau candidate class
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class RecTau: public ObjectBase, public FourMom
  {    
  public:

    RecTau(); 
    virtual ~RecTau() {}
    
    static std::string GetObjectType() { return "RecTau"; }

    int  GetCharge() const { return fCharge; }
    void SetCharge(int val) { fCharge = val; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:
   
    int fCharge;
  };
}

#endif
