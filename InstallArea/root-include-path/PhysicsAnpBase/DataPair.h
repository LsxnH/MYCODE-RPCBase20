// -*- c++ -*-
#ifndef ANP_DATAPAIR_H
#define ANP_DATAPAIR_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : DataPair
 * @Author : Rustem Ospanov
 *
 * @Brief  : Template for (key, data) pair
 *
 **********************************************************************************/

// C++
#include <iostream>
#include <string>
#include <vector>

namespace Anp
{   
  template<class K, class D> class DataPair
  {
  public:
    
    DataPair() {}
    DataPair(const K &key_, const D &data_);
    ~DataPair() {}
    
    void Set    (const K &key_, const D &data_);
    void SetData(const D &data_) { data = data_; }
    void SetKey (const K &key_)  { key  = key_; }
    
    const D& Data() const { return data; }
    const K& Key()  const { return key; }
    
    const D& GetData() const { return data; }
    const K& GetKey()  const { return key; }
    
   private:
    
    K key;
    D data;
  };

  //------------------------------------------------------------------------
  // Inlined member functions
  //------------------------------------------------------------------------
  template<class K, class D> DataPair<K, D>::DataPair(const K &key_, const D &data_) 
    :key(key_), data(data_) {}
  
  template<class K, class D> void DataPair<K, D>::Set(const K &key_, const D &data_) { 
    key  = key_;
    data = data_;
  }
  
  //------------------------------------------------------------------------
  // Inlined global functions
  //------------------------------------------------------------------------
  template<class K, class D>  inline bool operator < (const DataPair<K, D> &lhs, const DataPair<K, D> &rhs) 
  {
    return (lhs.Key() < rhs.Key());
  }
  template<class K, class D> inline bool operator < (const DataPair<K, D> &lhs, const K &rhs)
  {
    return (lhs.Key() < rhs);
  }
  template<class K, class D> inline bool operator < (const K &lhs, const DataPair<K, D> &rhs) 
  {
    return (lhs < rhs.Key());
  }
  
  template<class K, class D> inline bool operator == (const DataPair<K, D> &lhs, const DataPair<K, D> &rhs)
  {
    return (lhs.Key() == rhs.Key());
  }
  template<class K, class D> inline bool operator == (const DataPair<K, D> &lhs, const K &rhs)
  {
    return (lhs.Key() == rhs);
  }
  template<class K, class D> inline bool operator == (const K &lhs, const DataPair<K, D> &rhs)
  {
    return (lhs == rhs.Key());
  }

  template<class K, class D> inline bool operator != (const DataPair<K, D> &lhs, const DataPair<K, D> &rhs)
  {
    return (lhs.Key() != rhs.Key());
  }
  template<class K, class D> inline bool operator != (const DataPair<K, D> &lhs, const K &rhs)
  {
    return (lhs.Key() != rhs);
  }
  template<class K, class D> inline bool operator != (const K &lhs, const DataPair<K, D> &rhs)
  {
    return (lhs != rhs.Key());
  }
}

#endif
