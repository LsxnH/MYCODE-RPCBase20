// -*- c++ -*-
#ifndef ANP_RECHIT_H
#define ANP_RECHIT_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecHit
 * @Author : Rustem Ospanov
 *
 * @Brief  : Reconstructed detector hit in inner detector or muon system
 * 
 **********************************************************************************/

// C/C++
#include <vector>
#include <algorithm>
#include <cmath>

// Local
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{  
  class RecHit: public ObjectBase
  {    
  public:

    RecHit() {}
    virtual ~RecHit() {}
    
    static std::string GetObjectType() { return "RecElec"; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;
  };
}

#endif
