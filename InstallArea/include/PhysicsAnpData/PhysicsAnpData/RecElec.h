// -*- c++ -*-
#ifndef ANP_RECELEC_H
#define ANP_RECELEC_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecElec
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Reconstructed electron class
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class RecElec: public ObjectBase, public FourMom
  {
  public:    

    RecElec();
    virtual ~RecElec() {}

    static std::string GetObjectType() { return "RecElec"; }

    int  GetCharge() const { return fCharge; }    
    void SetCharge(int val) { fCharge = val; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:

    short fCharge;
  };
}

#endif
