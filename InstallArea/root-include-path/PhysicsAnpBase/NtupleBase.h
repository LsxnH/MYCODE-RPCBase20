// -*- c++ -*-
#ifndef ANP_NTUPLEBASE_H
#define ANP_NTUPLEBASE_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : AlgEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Helper classes to read/write ntuple data
 *
 **********************************************************************************/

//
// 
//

// ROOT
#include "TTree.h"

// C/C++
#include <iostream>
#include <string>

// Local
#include "PhysicsAnpBase/UtilBase.h"

namespace Anp
{
  //
  // State/type of ntuple branch
  //
  namespace Ntuple {
    enum Type { kNONE, kREAD };
  }

  //
  // Base class for ntuple branch manipulations
  //
  class NtupleBase {
  public:

    virtual ~NtupleBase() {}

    virtual NtupleBase* Clone() = 0;

    virtual void Copy(NtupleBase *rhs) = 0;
    
    void SetState(Ntuple::Type t) { fState = t; }

    Ntuple::Type GetState() const { return fState; }

    const std::string& GetType() const { return fType; }
    const std::string& GetName() const { return fName; }

  private:

    // These functions not implemented by design
    NtupleBase(NtupleBase &);
    NtupleBase &operator=(NtupleBase &);

  protected:

    NtupleBase() : fState(Ntuple::kNONE) {}  
    
    Ntuple::Type fState;       // State type 

    std::string  fType;        // Type T for NtupleData<T>
    std::string  fName;        // Branch name
  };

  //
  // Derived template class for ntuple branch manipulations
  //
  template <class T> class NtupleData : public NtupleBase 
  {
    public:

    NtupleData(T *data, bool own_ptr);
    NtupleData(T *data, const std::string &dtype, bool own_ptr);
    virtual ~NtupleData();
    
    NtupleBase* Clone();

    void Copy(NtupleBase *rhs);

    T* GetData() const { return  fData; }
    T* GetPtr()  const { return  fData; }
    T& GetRef()  const { return *fData; }
    
    void SetType(const std::string &type) { fType = type; }

  private:

    T              *fData;       //! Smart pointer to owned data
    bool            fOwnPtr;     //! Do I own data pointer?
  };
}

//
// Inline and templated functions
//
//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleData<T>::NtupleData(T *data, bool own_ptr)
  : NtupleBase(), fData(data) , fOwnPtr(own_ptr)
{
}

//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleData<T>::NtupleData(T *data, const std::string &dtype, bool own_ptr)
  : NtupleBase(), fData(data) , fOwnPtr(own_ptr)
{
  fType = dtype;
}

//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleData<T>::~NtupleData()
{
  if(fData && fOwnPtr) { delete fData; fData = 0; fOwnPtr = false; }
}

//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleBase* Anp::NtupleData<T>::Clone()
{
  NtupleData<T> *data = new NtupleData<T>(new T, true);
  data -> fName = fName;
  data -> fType = fType;

  return data;
}

//--------------------------------------------------------------------------------------
template<class T>
inline void Anp::NtupleData<T>::Copy(NtupleBase *rhs)
{
  if(!rhs || !fData) return;

  NtupleData<T> *data = dynamic_cast<NtupleData<T> *>(rhs);
  if(!data) {
    std::cerr << "NtupleData<T>::Copy - failed to cast" << std::endl;
    return;
  }
  if(!(data->fData)) {
    std::cerr << "NtupleData<T>::Copy - null data pointer" << std::endl;
    return;
  }

  (*fData) = *(data->fData);
  fState = data->fState;
}

#endif
