// -*- c++ -*-
#ifndef ANP_OBJECTBASE_H
#define ANP_OBJECTBASE_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : ObjectBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Base class for object data
 * 
 **********************************************************************************/

// C/C++
#include <iostream>
#include <map>
#include <string>
#include <typeinfo>

// Local
#include "PhysicsAnpData/Ptr.h"
#include "PhysicsAnpData/VarHolder.h"

namespace Anp
{
  //===========================================================================================
  class ObjectBase: public VarHolder
  {
  public:

    ObjectBase();
    virtual ~ObjectBase() {}

    void SetBarcode(uint64_t val)  { fBarcode = val; }
    
    uint64_t GetBarcode() const { return fBarcode; }

    template<class T> bool AddObjPtr(const std::string &key, const Ptr<T> &ptr);

    template<class T> Ptr<T> GetObjPtr(const std::string &key) const;

    bool HasObjPtr(const std::string &key) const;

    virtual void ClearObjectBase();

    virtual void Print(std::ostream &os = std::cout) const = 0;

  private:

    typedef std::map<std::string, Ptr<ObjectBase> > ObjMap;

  private:
   
    uint64_t fBarcode;
    ObjMap   fObjMap;
  };

  //===========================================================================================
  template <typename T> class ObjectVector: public ObjectBase
  {
  public:

    ObjectVector() {}
    explicit ObjectVector(const std::vector<Ptr<T> > &data);
    virtual ~ObjectVector() {}

    std::vector<Ptr<T> >& GetDataVec() { return fDataVec; }

    void ClearObjectBase();
    
    virtual void Print(std::ostream &os = std::cout) const;

  private:
    
    std::vector<Ptr<T> > fDataVec;
  };

  //===========================================================================================
  // inlined functions
  //
  inline ObjectBase::ObjectBase():
    fBarcode(0)
  {
  }
  
  //===========================================================================================
  inline void ObjectBase::ClearObjectBase()
  {
    VarHolder::ClearVars();

    for(ObjMap::value_type &v: fObjMap) {
      v.second->ClearObjectBase();
    }
    
    fObjMap.clear();

    fBarcode = 0;
  }

  //===========================================================================================
  template<class T> inline bool ObjectBase::AddObjPtr(const std::string &key, const Ptr<T> &ptr)
  {
    Ptr<ObjectBase> optr = Anp::DynamicCastPtr<ObjectBase, T>(ptr);

    if(!optr.valid()) {
      std::cout << "ObjectBase::AddObjPtr - failed to cast pointer with key=" << key << std::endl;
      return false;
    }

    if(!fObjMap.insert(ObjMap::value_type(key, optr)).second) {
      std::cout << "ObjectBase::AddObjPtr - failed to insert pointer with key=" << key << std::endl;
      return false;
    }
    
    return true;
  }

  //===========================================================================================
  template<class T> inline Ptr<T> ObjectBase::GetObjPtr(const std::string &key) const
  {
    const ObjMap::const_iterator fit = fObjMap.find(key);

    if(fit == fObjMap.end()) {
      std::cout << "ObjectBase::GetObjPtr - unknown key=" << key << std::endl;
      return Ptr<T>();
    }
    
    return Anp::DynamicCastPtr<T, ObjectBase>(fit->second);
  }

  //===========================================================================================
  inline bool ObjectBase::HasObjPtr(const std::string &key) const
  {
    return fObjMap.find(key) != fObjMap.end();
  }

  //===========================================================================================
  template <typename T> inline ObjectVector<T>::ObjectVector(const std::vector<Ptr<T> > &data):
    fDataVec(data)
  {
  }

  //===========================================================================================
  template <typename T> inline void ObjectVector<T>::ClearObjectBase()
  {
    //
    // Call base class function
    //
    ObjectBase::ClearObjectBase();

    //
    // Release all smart pointers
    //
    fDataVec.clear();
  }
  
  //==============================================================================
  template <typename T> inline void ObjectVector<T>::Print(std::ostream &os) const
  {
    os << "ObjectVector<" << typeid(*this).name() << "> - data size = " << fDataVec.size() << std::endl;
    
    for(const Ptr<T> &ptr: fDataVec) {
      ptr->Print(os);
    }
  }  
}

#endif
