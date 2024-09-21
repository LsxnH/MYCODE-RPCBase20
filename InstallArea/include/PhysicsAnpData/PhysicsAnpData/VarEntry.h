// -*- c++ -*-
#ifndef ANP_VARENTRY_H
#define ANP_VARENTRY_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : VarEntry
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 * One floating variable with unsigned integer key
 *
 **********************************************************************************/

// ROOT
#include "RtypesCore.h"

// C/C++
#include <stdint.h>

namespace Anp
{
  class VarEntry
  {
  public:
    
    VarEntry();
    VarEntry(unsigned key, double    value);
    VarEntry(unsigned key, Long64_t  value);
    VarEntry(unsigned key, ULong64_t value);
    ~VarEntry() {}
    
    unsigned key   () const { return fKey; }
    unsigned GetKey() const { return fKey; }

    double    GetData    () const { return fDataD;   }
    double    GetValue   () const { return fDataD;   }
    Long64_t  GetValueL64() const { return fDataL64; }
    ULong64_t GetValueU64() const { return fDataU64; }
    
    bool IsValidDouble() const { return fStatus == 1; }
    bool IsValidL64   () const { return fStatus == 2; }
    bool IsValidU64   () const { return fStatus == 3; }

  private:
    
    uint32_t  fKey;      // variable key
    short     fStatus;   // variable statues

    double    fDataD;    // variable value: double
    Long64_t  fDataL64;  // variable value: 64 bit unsigned integer
    ULong64_t fDataU64;  // variable value: 64 bit unsigned integer
  };
  
  //
  // Inlined comparison operators
  //
  inline bool operator==(const VarEntry &lhs, const VarEntry &rhs) { 
    return lhs.GetKey() == rhs.GetKey();
  }
  inline bool operator <(const VarEntry &lhs, const VarEntry &rhs) { 
    return lhs.GetKey() < rhs.GetKey();
  }

  inline bool operator==(const VarEntry &var, unsigned key) { return var.GetKey() == key; }
  inline bool operator==(unsigned key, const VarEntry &var) { return var.GetKey() == key; }

  inline bool operator<(const VarEntry &var, unsigned key) { return var.GetKey() < key; }
  inline bool operator<(unsigned key, const VarEntry &var) { return key < var.GetKey(); }
}

#endif
