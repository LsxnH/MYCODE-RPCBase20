// This is -*- c++ -*-
#ifndef ANP_READUTILS_H
#define ANP_READUTILS_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : ReadUtils
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 * Helper functions, classes and templates for reading flat trees
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <set>
#include <vector>

// Data
#include "PhysicsAnpData/VarHolder.h"

// Base
#include "PhysicsAnpBase/Registry.h"

class TBranch;
class TFile;
class TLeaf;
class TTree;

namespace Anp
{
  //==============================================================================
  // Helper class to reading flat tree branches
  //==============================================================================
  namespace Read
  {
    enum Type {
      kNone = 0,
      kFloat,
      kDouble,
      kShort,
      kInt,
      kLong,
      kLong64,
      kUShort,
      kUInt,
      kULong,
      kULong64,
      kBool,
      kChar,
      kUChar,

      kVecFloat,
      kVecDouble,
      kVecShort,
      kVecInt,
      kVecLong,
      kVecUShort,
      kVecUInt,
      kVecULong,
      kVecULong64,
      kVecBool,
      kVecChar,
      kVecUChar,

      kVecVecFloat,
      kVecVecInt,
      kVecVecU64
    };

    Type GetBranchType(TBranch *branch);
    
    std::string GetBranchTypeAsStr(Type t);

    bool IsFlat        (Type type);
    bool IsVector      (Type type);
    bool IsVectorVector(Type type);
  }

  //==============================================================================
  // Helper class to reading flat tree branches
  //==============================================================================
  struct VarData 
  {
    VarData();
    ~VarData() {}
    
    void Clear();

    void Print() const;
    
    std::string GetVarValAsStr() const;
    
    template<class T> bool AddVarVal(T &obj) const;

    bool SetVarBranch(TTree *tree, bool debug=false);

    template<typename T, class Y> void CheckVarVal(T val, const Y &obj) const;

    std::string  key;
    std::string  branch;
    std::string  nick;

    unsigned     var;
    Read::Type   type;
    bool         valid;

    Float_t               val_flt;
    Double_t              val_dbl;
    Short_t               val_snt;
    Int_t                 val_int;
    Long_t                val_lnt;
    Long64_t              val_l64;
    UShort_t              val_usn;
    UInt_t                val_unt;
    ULong_t               val_uln;
    ULong64_t             val_u64;
    Bool_t                val_bln;
    Char_t                val_chr;
    UChar_t               val_uch;
  };

  //==============================================================================
  // Helper class to reading vector<T> or vector<vector<T> > tree branches
  //==============================================================================
  struct VecData 
  {
    VecData();
    ~VecData() {}
    
    void Clear();

    void Delete();

    void Print() const;
    
    bool SetVecBranch(TTree *tree);

    unsigned GetVectorSize() const;

    unsigned GetDoubleVectorSize(unsigned index) const;

    double GetVecValAt(unsigned index) const;
    
    bool AddVecVal(VarHolder &obj, unsigned index) const;

    bool AddVecVecVal(VarHolder &obj, unsigned outer_index, unsigned inner_index) const;

    std::string  key;
    std::string  branch;
    Read::Type   branch_type;
    unsigned     var;
    bool         debug;

    std::vector<Float_t>       *vec_flt;
    std::vector<Double_t>      *vec_dbl;
    std::vector<Short_t>       *vec_snt;
    std::vector<Int_t>         *vec_int;
    std::vector<Long_t>        *vec_lnt;
    std::vector<UShort_t>      *vec_usn;
    std::vector<UInt_t>        *vec_unt;
    std::vector<ULong_t>       *vec_uln;
    std::vector<ULong64_t>     *vec_u64;
    std::vector<Bool_t>        *vec_bln;
    std::vector<char>          *vec_chr;
    std::vector<unsigned char> *vec_uch;

    std::vector<std::vector<float> >     *vvv_flt;
    std::vector<std::vector<int> >       *vvv_int;
    std::vector<std::vector<ULong64_t> > *vvv_u64;
  };

  //==============================================================================
  // Helper class to parse variable nicknames
  //==============================================================================
  struct VarNick
  {
    VarNick() :key(Var::NONE) {}
    
    std::string nick;
    std::string var;      
    unsigned    key;
  };

  std::vector<VarNick> ReadNicks(const std::string &key, const Registry &reg);

  //==============================================================================
  // Inlined class member templates
  //==============================================================================
  template<class T> bool VarData::AddVarVal(T &obj) const
  { 
    if     (type == Read::kDouble ) {                            return obj.AddVar   (var, val_dbl); }
    else if(type == Read::kFloat  ) {                            return obj.AddVar   (var, val_flt); }
    else if(type == Read::kInt    ) { CheckVarVal(val_int, obj); return obj.AddVar   (var, val_int); }
    else if(type == Read::kShort  ) {                            return obj.AddVar   (var, val_snt); }
    else if(type == Read::kLong   ) { CheckVarVal(val_lnt, obj); return obj.AddVar   (var, val_lnt); }
    else if(type == Read::kLong64 ) { CheckVarVal(val_l64, obj); return obj.AddVar   (var, val_l64); }
    else if(type == Read::kBool   ) {                            return obj.AddVar   (var, val_bln); }
    else if(type == Read::kChar   ) {                            return obj.AddVar   (var, val_chr); }
    else if(type == Read::kUChar   ) {                           return obj.AddVar   (var, val_uch); }
    else if(type == Read::kUInt   ) { CheckVarVal(val_unt, obj); return obj.AddVar   (var, val_unt); }
    else if(type == Read::kULong64) {                            return obj.AddVarU64(var, val_u64); }
    
    std::cout << "VarData::AddVarVal<" << obj.GetObjectType() 
	      << "> - missing value for: " << Var::AsStr(var)  
	      << " type=" << Read::GetBranchTypeAsStr(type) << " valid=" << valid << std::endl;

    return false;
  }

  //==============================================================================
  template<typename T, class Y> void VarData::CheckVarVal(T val, const Y &obj) const
  {
    if(static_cast<T>(double(val)) != val) {
      static unsigned icount = 0;
      
      if(++icount < 1000) {
	std::cout << "CheckVarVal - " << obj.GetObjectType() 
		  << " - loss of precision for var=\"" << Var::AsStr(var) << "\""
		  << " type=" << Read::GetBranchTypeAsStr(type) << " valid=" << valid 
		  << ": " << static_cast<T>(double(val)) << "!=" << val << std::endl;
      }
      else if(icount == 1000) {
	std::cout << "   ...last warning" << std::endl;
      }
    }
  }

  //==============================================================================
  // Split path using "/" separator
  //
  std::pair<std::string, std::string> inline SplitPath(const std::string &file, bool remove_trailing_slash = true) 
  {
    std::pair<std::string, std::string> res("", file);

    const size_t ipos = file.find_last_of('/');

    if(ipos != std::string::npos) {
      res.first  = file.substr(0, ipos+1);
      res.second = file.substr(ipos+1, std::string::npos);
      
      if(remove_trailing_slash && res.first.size() > 0 && res.first.at(res.first.size()-1) == '/') {
	res.first = res.first.substr(0, res.first.size()-1);	
      }
    }
    
    return res;
  }

  //==============================================================================
  // Helper functions for reading trees
  //
  template<class T> inline TBranch* SetBranch(TTree *tree, const std::string &branch, T &var)
  {
    if(!tree) {
      return 0;
    }
    
    TBranch *b = 0;

    if(tree->GetBranch(branch.c_str())) {
      tree->SetBranchStatus (branch.c_str(), 1);
      tree->SetBranchAddress(branch.c_str(), &var, &b); 
    }

    return b;
  }

  //==============================================================================
  template<class T> inline TBranch* SetBranch(TTree *tree, const std::string &branch, T *&var)
  {
    if(!tree) {
      return 0;
    }

    TBranch *b = 0;
    
    if(tree->GetBranch(branch.c_str())) {
      tree->SetBranchStatus (branch.c_str(), 1);
      tree->SetBranchAddress(branch.c_str(), &var, &b); 
    }

    return b;
  }
}

#endif
