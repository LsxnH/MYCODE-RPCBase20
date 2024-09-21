// -*- c++ -*-
#ifndef ANP_NTUPLESVC_H
#define ANP_NTUPLESVC_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : NtupleSvc
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  NtuplSvc is singleton class that controls read/write access to TTree branches.
 * 
 *  This method allows READ/WRITE access to branches:
 *    - Branch<T>(const std::string &, const std::string &, T *)
 * 
 *  This method allows READ only access to single branch matching type T:
 *    - ReadBranch<T>(const std::string &branch);
 * 
 **********************************************************************************/

// C/C++
#include <cassert>
#include <vector>

// Local
#include "PhysicsAnpBase/Factory.h"
#include "PhysicsAnpBase/NtupleBase.h"

namespace Anp
{
  class Registry;

  template <class T> class Branch 
  {
  public:   

    Branch() :fData(0) {}
    explicit Branch(NtupleData<T> *data) : fData(data) { assert(fData && "null branch pointer"); }
    ~Branch() {}

    bool IsValid() const { return fData; }
    bool   valid() const { return fData; }

    bool IsValidEvent() const 
    { 
      if(fData && fData->GetState()==Ntuple::kREAD) {
	return true; 
      }
      return false; 
    }

    void release() { fData = 0; }

    T* GetData() { return fData->GetData(); }
    T* GetPtr()  { return fData->GetPtr();  }
    T& GetRef()  { return fData->GetRef();  }

    const T& GetRef() const { return fData->GetRef(); }

    T* get()  { return fData->GetPtr(); }
    T& ref()  { return fData->GetRef(); }

    T* operator->() { return fData->GetPtr(); }
    T& operator *() { return fData->GetRef(); }

    const T* operator->() const { return fData->GetPtr(); }
    const T& operator *() const { return fData->GetRef(); }

    void SetState(Ntuple::Type t) { fData->SetState(t); }

    Ntuple::Type GetState() { return fData->GetState(); }
    
    void SetType(const std::string &type) { fData->SetType(type); }

  private:

    NtupleData<T> *fData;
  };

  class NtupleSvc {
  public:

    enum Mode { NONE, READ, FILL, NEW_ENTRY };

    static NtupleSvc& Instance(const std::string &tree_name);

    void SetFile(TFile *tree);    
    bool SetTree(TTree *tree, Mode mode);

    bool Set(Mode mode);

    bool Reset();

    void ConfigFrom(NtupleSvc &rhs);

    void CopyTo(NtupleSvc &rhs);

    void Config(const Registry &reg);

    void Print() const;
    
    TFile *GetFile() const { return fFile; }
    TTree *GetTree() const { return fTree; }
    Mode   GetMode() const { return fMode; }

    template<class T> Anp::Branch<T> Branch(const std::string &branch);
    
    void SetBranchStatus(const std::string &branch, bool status);

    void EnableDebug() { fDebug = true; }

  private:

    // These functions are private by design
    NtupleSvc();
    ~NtupleSvc() {}    
    
    NtupleSvc(const NtupleSvc &);
    NtupleSvc& operator=(const NtupleSvc &);

    std::ostream& log();

  private:

    struct Info {
      bool status;
    };

    typedef std::map<std::string, Anp::NtupleBase *> BranchMap;
    typedef std::map<std::string, Info> InfoMap;
    
  private:

    TFile            *fFile;
    TTree            *fTree;
    std::string       fName;

    Mode              fMode;
    BranchMap         fBranchMap;
    InfoMap           fInfoMap;

    bool              fBranchStatusOff;  // disable all branches by default
    bool              fDebug;            // print debug messages
  };

  //
  // Helper functions
  //
  template<class T> void ClearBranch(const std::string &tree,
				     const std::string &branch);
}

//
// Implementation details
//
#include "NtupleSvc.icc"
#endif
