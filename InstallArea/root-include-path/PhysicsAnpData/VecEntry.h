// -*- c++ -*-
#ifndef ANP_VECENTRY_H
#define ANP_VECENTRY_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : VecEntry
 * @Author : Rustem Ospanov
 *
 * @Brief  : Vector of variables stored with integer key
 *
 **********************************************************************************/

// C/C++
#include <stdint.h>
#include <iostream>
#include <vector> 

namespace Anp
{
  template <typename T> class VecEntry
  {
  public:
    
    VecEntry();
    VecEntry(uint32_t key, const std::vector<T> &vec);
    ~VecEntry() {}
    
    void SetKey(uint32_t              key) { fKey = key; }
    void SetVec(const std::vector<T> &vec) { fVec = vec; }

    unsigned              GetKey() const { return fKey; }
    const std::vector<T>& GetVec() const { return fVec; }
    
  private:
    
    uint32_t       fKey;    // variable key
    std::vector<T> fVec;    // vector data
  };
 
  //
  // Global typedefs
  //
  typedef std::vector<VecEntry<int> >   IntEntryVec;
  typedef std::vector<VecEntry<float> > FloatEntryVec;

  //
  // Inlined comparison operators
  //
  template <typename T> bool operator==(const VecEntry<T> &lhs, const VecEntry<T> &rhs) { 
    return lhs.GetKey() == rhs.GetKey();
  }
  template <typename T> bool operator <(const VecEntry<T> &lhs, const VecEntry<T> &rhs) { 
    return lhs.GetKey() < rhs.GetKey();
  }
  
  template <typename T> bool operator==(const VecEntry<T> &var, unsigned key) { return var.GetKey() == key; }
  template <typename T> bool operator==(unsigned key, const VecEntry<T> &var) { return var.GetKey() == key; }

  template <typename T> bool operator!=(const VecEntry<T> &var, unsigned key) { return var.GetKey() != key; }
  template <typename T> bool operator!=(unsigned key, const VecEntry<T> &var) { return var.GetKey() != key; }

  template <typename T> bool operator<(const VecEntry<T> &var, unsigned key) { return var.GetKey() < key; }
  template <typename T> bool operator<(unsigned key, const VecEntry<T> &var) { return key < var.GetKey(); } 

  //
  // Template implementation
  //
  template <typename T>
    VecEntry<T>::VecEntry()
    :fKey(0), fVec()
  {
  }

  template <typename T>
    VecEntry<T>::VecEntry(uint32_t key, const std::vector<T> &vec)
    :fKey(key), fVec(vec)
  {
  }
}

#endif
