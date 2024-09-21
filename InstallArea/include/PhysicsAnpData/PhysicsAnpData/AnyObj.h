// -*- c++ -*-
#ifndef ANP_ANYOBJ_H
#define ANP_ANYOBJ_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : AnyObj
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  AnyObj is generic object that can be used when existing objects are not enough
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/ObjectBase.h"
#include "PhysicsAnpData/Ptr.h"

namespace Anp
{
  class AnyObj: public ObjectBase
  {    
  public:

    AnyObj() {}
    virtual ~AnyObj() {}
    
    static std::string GetObjectType() { return "AnyObj"; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;
  };
}

#endif
