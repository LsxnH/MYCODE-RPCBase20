// -*- c++ -*-
#ifndef ANP_RECJET_H
#define ANP_RECJET_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecJet
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Reconstructed jet class
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class RecJet: public ObjectBase, public FourMom
  {
  public:

    RecJet();
    virtual ~RecJet() {}
    
    static std::string GetObjectType() { return "RecJet"; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;
  };
}

#endif
