// -*- c++ -*-
#ifndef ANP_RECTRACKJET_H
#define ANP_RECTRACKJET_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecJet
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Reconstructed track jet class
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class RecTrackJet: public ObjectBase, public FourMom
  {    
  public:

    RecTrackJet(); 
    virtual ~RecTrackJet() {}
    
    static std::string GetObjectType() { return "RecTrackJet"; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;
  };
}

#endif
