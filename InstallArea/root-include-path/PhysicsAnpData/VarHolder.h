// -*- c++ -*-
#ifndef ANP_VARHOLDER_H
#define ANP_VARHOLDER_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : VarHolder
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  VarHolder is candidate analysis object created by algorithms
 *  
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <iostream>
#include <set>
#include <vector> 

// Local
#include "PhysicsAnpData/VarEntry.h"
#include "PhysicsAnpData/VecEntry.h"

namespace Anp 
{
  typedef std::vector<Anp::VarEntry> VarEntryVec;

  //===============================================================================================================
  namespace Var
  {
    const unsigned NONE = 0;

    bool RegisterVar(unsigned var, const std::string &name);

    unsigned RegisterDynamicVar(const std::string &name);
    
    const std::string& AsStr         (unsigned var);
    const std::string  AsStr         (unsigned var, double value);
    const std::string& ConvertVar2Str(unsigned var);
    unsigned ConvertStr2Var(const std::string &var); 
    unsigned Convert2Var   (const std::string &var); 
    
    bool IsKnownVar(unsigned var);
    bool IsKnownVar(const std::string &name);

    const std::vector<std::string>& GetAllVarNames();
    const std::vector<unsigned>&    GetAllVarKeys ();
    
    void PrintAllVars();

    void PrintDynamicVars(bool do_python_format=false);

    void ThrowIfDebug(const std::string &msg);
  }

  //
  // Pre-processor macro to register variables - use inside Var namespace
  //
  #define REGISTER_ANP_VAR(VAR) RegisterVar( VAR, #VAR )
  
  //===============================================================================================================
  class VarHolder 
  {
  public:

    VarHolder() {}
    virtual ~VarHolder() {}
    
    bool  AddVar    (unsigned key, double    value);
    bool  AddVarL64 (unsigned key, Long64_t  value);
    bool  AddVarU64 (unsigned key, ULong64_t value);

    bool  AddVec    (unsigned key, const std::vector<int>       &vec);
    bool  AddVec    (unsigned key, const std::vector<float>     &vec);
    bool  AddVecL64 (unsigned key, const std::vector<Long64_t>  &vec);
    bool  AddVecU64 (unsigned key, const std::vector<ULong64_t> &vec);
    bool  AddVec    (unsigned key, const std::vector<VarHolder> &vec);

    bool  ReplaceVar(unsigned key, double value);
    bool  DelVar    (unsigned key);
    bool  DelVec    (unsigned key);

    int CopyVarsFrom(const VarHolder &input);
    int CopyVarsFrom(const VarHolder &input, const std::set<unsigned> &filter);

    int      GetInt(unsigned key, int      defval) const;
    Long64_t GetL64(unsigned key, Long64_t defval) const;
    uint64_t GetU64(unsigned key, uint64_t defval) const;
    double   GetDbl(unsigned key, double   defval) const;

    bool GetVar   (unsigned key, double    &value) const;
    bool GetVar   (unsigned key, float     &value) const;
    bool GetVar   (unsigned key, int       &value) const;
    bool GetVarL64(unsigned key, Long64_t  &value) const;
    bool GetVarU64(unsigned key, ULong64_t &value) const;
    bool GetVarU64(unsigned key, uint64_t  &value) const;

    bool GetVarVec   (unsigned key, std::vector<int>       &value) const;
    bool GetVarVec   (unsigned key, std::vector<float>     &value) const;
    bool GetVarVecL64(unsigned key, std::vector<Long64_t>  &value) const;
    bool GetVarVecU64(unsigned key, std::vector<ULong64_t> &value) const;
    bool GetVarVec   (unsigned key, std::vector<VarHolder> &value) const;

    bool HasKey(unsigned key) const;
    bool HasVar(unsigned key) const;
    bool HasVec(unsigned key) const;

    unsigned GetNVars() const { return fVars.size(); }

    const VarEntryVec& GetVars() const { return fVars; }

    const std::vector<VecEntry<VarHolder> >& GetVarHolderVecs() const { return fHolders; }

    void ClearVars();
    
    std::string GetVarsAsStr(const std::string &pad="") const;

  private:

    typedef std::vector<VecEntry<int> >       IntVec;
    typedef std::vector<VecEntry<float> >     FltVec;
    typedef std::vector<VecEntry<Long64_t> >  L64Vec;
    typedef std::vector<VecEntry<ULong64_t> > U64Vec;
    typedef std::vector<VecEntry<VarHolder> > HolderVec;

  private:

    VarEntryVec     fVars;

    IntVec          fInts;
    FltVec          fFloats;
    L64Vec          fVecL64;
    U64Vec          fVecU64;
    HolderVec       fHolders;
  };

  //===============================================================================================================
  // Inlined functions
  //===============================================================================================================
  inline bool Anp::VarHolder::ReplaceVar(const unsigned key, const double value)
  {
    if(!HasKey(key)) {
      fVars.push_back(Anp::VarEntry(key, value));
      return true;
    }
    else{
      DelVar(key);
      AddVar(key, value);
    }
    
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::AddVar(const unsigned key, const double value)
  {
    if(!HasKey(key)) {
      fVars.push_back(Anp::VarEntry(key, value));
      return true;
    }

    Var::ThrowIfDebug("VarHolder::AddVar(" + std::to_string(key) + ", " + std::to_string(value) + ") - key already exists");

    return false;
  }

 //===============================================================================================================
  inline bool Anp::VarHolder::AddVarL64(const unsigned key, const Long64_t value)
  {
    if(!HasKey(key)) {
      fVars.push_back(Anp::VarEntry(key, value));
      return true;
    }

    Var::ThrowIfDebug("VarHolder::AddVarL64(" + std::to_string(key) + ", " + std::to_string(value) + ") - key already exists");

    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::AddVarU64(const unsigned key, const ULong64_t value)
  {
    if(!HasKey(key)) {
      fVars.push_back(Anp::VarEntry(key, value));
      return true;
    }
    
    Var::ThrowIfDebug("VarHolder::AddVarU64(" + std::to_string(key) + ", " + std::to_string(value) + ") - key already exists");

    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::AddVec(const unsigned key, const std::vector<int> &vec)
  {
    if(!HasKey(key)) {
      fInts.push_back(Anp::VecEntry<int>(key, vec));
      return true;
    }
    
    std::cout << "VarHolder::AddVec(" << key << ") - key already exists" << std::endl;
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::AddVec(const unsigned key, const std::vector<float> &vec)
  {    
    if(!HasKey(key)) {
      fFloats.push_back(Anp::VecEntry<float>(key, vec));
      return true;
    }
    
    std::cout << "VarHolder::AddVec(" << key << ") - key already exists" << std::endl;
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::AddVecL64(const unsigned key, const std::vector<Long64_t> &vec)
  {    
    if(!HasKey(key)) {
      fVecL64.push_back(Anp::VecEntry<Long64_t>(key, vec));
      return true;
    }
    
    std::cout << "VarHolder::AddVecL64(" << key << ") - key already exists" << std::endl;
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::AddVecU64(const unsigned key, const std::vector<ULong64_t> &vec)
  {    
    if(!HasKey(key)) {
      fVecU64.push_back(Anp::VecEntry<ULong64_t>(key, vec));
      return true;
    }
    
    std::cout << "VarHolder::AddVecU64(" << key << ") - key already exists" << std::endl;
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::AddVec(const unsigned key, const std::vector<VarHolder> &vec)
  {    
    if(!HasKey(key)) {
      fHolders.push_back(Anp::VecEntry<VarHolder>(key, vec));
      return true;
    }
    
    std::cout << "VarHolder::AddVec(" << key << ") - key already exists" << std::endl;
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::DelVar(const unsigned key)
  {    
    VarEntryVec::iterator vit = fVars.begin();
    while(vit != fVars.end()) {
      if(vit->GetKey() == key) {
	vit = fVars.erase(vit);
      }
      else {
	vit++;
      }
    }
    
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::DelVec(const unsigned key)
  {    
    IntVec::iterator vit = fInts  .begin();
    FltVec::iterator fit = fFloats.begin();

    while(vit != fInts.end()) {
      if(vit->GetKey() == key) {
	vit = fInts.erase(vit);
      }
      else {
	vit++;
      }
    }

    while(fit != fFloats.end()) {
      if(fit->GetKey() == key) {
	fit = fFloats.erase(fit);
      }
      else {
	fit++;
      }
    }
    
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::HasKey(unsigned key) const
  {
    return 
      std::find(fVars   .begin(), fVars   .end(), key) != fVars   .end() ||
      std::find(fInts   .begin(), fInts   .end(), key) != fInts   .end() ||
      std::find(fFloats .begin(), fFloats .end(), key) != fFloats .end() ||
      std::find(fVecL64 .begin(), fVecL64 .end(), key) != fVecL64 .end() ||
      std::find(fVecU64 .begin(), fVecU64 .end(), key) != fVecU64 .end() ||
      std::find(fHolders.begin(), fHolders.end(), key) != fHolders.end();
  }
    
  //===============================================================================================================
  inline bool Anp::VarHolder::HasVec(unsigned key) const
  {
    return 
      std::find(fInts   .begin(), fInts   .end(), key) != fInts   .end() ||
      std::find(fFloats .begin(), fFloats .end(), key) != fFloats .end() ||
      std::find(fVecL64 .begin(), fVecL64 .end(), key) != fVecL64 .end() ||
      std::find(fVecU64 .begin(), fVecU64 .end(), key) != fVecU64 .end() ||
      std::find(fHolders.begin(), fHolders.end(), key) != fHolders.end();
  }
  //===============================================================================================================
  inline bool Anp::VarHolder::HasVar(unsigned key) const
  {
    return std::find(fVars.begin(), fVars.end(), key) != fVars.end();
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::GetVar(unsigned key, float &value) const
  {
    //
    // Read variable
    //
    const VarEntryVec::const_iterator ivar = std::find(fVars.begin(), fVars.end(), key);
    if(ivar != fVars.end()) {
      value = ivar->GetValue();
      return true;
    }
    
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::GetVar(unsigned key, double &value) const
  {
    //
    // Read variable
    //
    const VarEntryVec::const_iterator ivar = std::find(fVars.begin(), fVars.end(), key);
    if(ivar != fVars.end()) {
      value = ivar->GetValue();
      return true;
    }
    
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::GetVar(unsigned key, int &value) const
  {
    //
    // Read variable
    //
    double val = 0.0;
    
    if(!GetVar(key, val)) {
      return false;
    }
    
    value = static_cast<int>(val);
    
    if(std::fabs(val - double(value)) > 0.0) {
      std::cout << "GetVar - error converting double to int: " << val << " != " << value << std::endl;
      return false;
    }
    
    return true;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarL64(unsigned key, Long64_t &value) const
  {
    const VarEntryVec::const_iterator vit = std::find(fVars.begin(), fVars.end(), key);

    if(vit != fVars.end() && vit->IsValidL64()) {
      value = vit->GetValueL64();
      return true;
    }
    
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarU64(unsigned key, ULong64_t &value) const
  {
    const VarEntryVec::const_iterator vit = std::find(fVars.begin(), fVars.end(), key);

    if(vit != fVars.end() && vit->IsValidU64()) {
      value = vit->GetValueU64();
      return true;
    }
    
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarU64(unsigned key, uint64_t &value) const
  {
    //
    // Provide this function since type(ULong64_t) != type(uint64_t)
    //
    ULong64_t uval = value;
    
    if(GetVarU64(key, uval)) {
      value = static_cast<uint64_t>(uval);
      return true;
    }
    
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarVec(unsigned key, std::vector<int> &value) const
  {
    const std::vector<VecEntry<int> >::const_iterator ivar = std::find(fInts.begin(), fInts.end(), key);
    if(ivar != fInts.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarVec(unsigned key, std::vector<float> &value) const
  {
    const std::vector<VecEntry<float> >::const_iterator ivar = std::find(fFloats.begin(), fFloats.end(), key);
    if(ivar != fFloats.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }

  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarVecL64(unsigned key, std::vector<Long64_t> &value) const
  {
    const L64Vec::const_iterator ivar = std::find(fVecL64.begin(), fVecL64.end(), key);

    if(ivar != fVecL64.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }
  
  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarVecU64(unsigned key, std::vector<ULong64_t> &value) const
  {
    const U64Vec::const_iterator ivar = std::find(fVecU64.begin(), fVecU64.end(), key);

    if(ivar != fVecU64.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }
    
  //===============================================================================================================
  inline bool Anp::VarHolder::GetVarVec(unsigned key, std::vector<VarHolder> &value) const
  {
    const HolderVec::const_iterator ivar = std::find(fHolders.begin(), fHolders.end(), key);

    if(ivar != fHolders.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }

  //===============================================================================================================
  inline int Anp::VarHolder::GetInt(const unsigned key, const int defval) const
  {
    //
    // Find and return, if exists, value stored at key
    //
    double dval = -1.0e9;
    int    ival = defval;

    if(GetVar(key, dval)) {
      ival = static_cast<int>(dval);

      if(std::fabs(dval - static_cast<double>(ival)) > 0.0) {
	std::cout << "VarHolder::GetInt - rounding error when casting double to int" << std::endl;
      }
    }

    return ival;
  }

  //===============================================================================================================
  inline Long64_t Anp::VarHolder::GetL64(const unsigned key, const Long64_t defval) const
  {
    //
    // Find and return, if exists, value stored at key
    //
    Long64_t val = defval;

    GetVarL64(key, val);

    return val;
  }

  //===============================================================================================================
  inline uint64_t Anp::VarHolder::GetU64(const unsigned key, const uint64_t defval) const
  {
    //
    // Find and return, if exists, value stored at key
    //
    uint64_t val = defval;

    GetVarU64(key, val);

    return val;
  }

  //===============================================================================================================
  inline double Anp::VarHolder::GetDbl(const unsigned key, const double defval) const
  {
    //
    // Find and return, if exists, value stored at key
    //
    double val = defval;

    GetVar(key, val);

    return val;
  }
}

#endif
