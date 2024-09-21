// -*- c++ -*-
#ifndef ANP_UTILOBJS_H
#define ANP_UTILOBJS_H

// C/C++
#include <vector>

// Data
#include "PhysicsAnpData/Ptr.h"
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/VarHolder.h"

namespace Anp
{  
  //======================================================================================================
  // Sorting helpers
  //======================================================================================================
  struct SortObjectByPt
  {
    template<class T> bool operator()(const Ptr<T> &lhs, const Ptr<T> &rhs);
  };

  //======================================================================================================
  struct SortObjectByEnergy
  {
    template<class T> bool operator()(const Ptr<T> &lhs, const Ptr<T> &rhs);
  };

  //======================================================================================================
  struct SortObjectByMass
  {
    template<class T> bool operator()(const Ptr<T> &lhs, const Ptr<T> &rhs);
  };

  //======================================================================================================
  struct SortObjectByDR
  {
    explicit SortObjectByDR(const FourMom &p_):p(p_) {}
    
    template<class T> bool operator()(const Ptr<T> &lhs, const Ptr<T> &rhs);

  private:

    SortObjectByDR();
    FourMom p;
  };

  //======================================================================================================
  struct SortObjectByVar
  {
    explicit SortObjectByVar(const unsigned v):var(v) {}
    
    template<class T> bool operator()(const Ptr<T> &lhs, const Ptr<T> &rhs);

  private:

    SortObjectByVar();
    unsigned var;
  };
}

//======================================================================================================
// Include implementation details
//
#include "UtilObjs.icc"

#endif
