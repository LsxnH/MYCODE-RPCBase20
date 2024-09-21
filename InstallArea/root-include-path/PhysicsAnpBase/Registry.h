// -*- c++ -*-
#ifndef ANP_REGISTRY_H
#define ANP_REGISTRY_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : Registry
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *
 * Registry class helps with configuration of job within PhysicsNtuple 
 * framework. Registry configuration is set in python script or xml file
 * and then passed to RunModule and all AlgEvent instances created by a job.
 * 
 * 
 * Class stores (key, value) pairs for following types for values:
 *  - string
 *  - double
 *  - Registry
 *
 *  - provides int, unsigned, const char * Get() functions which are 
 *    internally converted to/from the above three basic types
 *
 *  - provides access methods for reading ',' separated lists as vectors
 *
 *  Use partial template implementation of Get/Set functions.
 *
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

// Local
#include "PhysicsAnpBase/DataPair.h"
#include "PhysicsAnpBase/UtilBase.h"

namespace Anp
{
  class Registry
  {
  public:

    typedef Anp::DataPair<std::string, std::string>   StrData;
    typedef Anp::DataPair<std::string, long double>   DblData;
    typedef Anp::DataPair<std::string, Anp::Registry> RegData;
    typedef Anp::DataPair<uint64_t,    uint64_t>      U64Data;

  public:
    
    Registry() :fUniqueKeys(true) {}
    ~Registry() {}

    // Deep copy of rhs into this Registry
    void Merge(const Registry& rhs);

    // Deep copy of rhs into this Registry: append to existing string values
    void Add(const Registry& rhs);
    
    bool KeyExists(const std::string &key) const;
    bool RemoveKey(const std::string &key);
    
    void AllowNonUniqueKeys() { fUniqueKeys = false; }

    // Full clear of this Registry 
    void Clear();
        
    //
    // Non-templated functions for python iterface
    //
    void SetVal(const std::string &key, bool                   val);
    void SetVal(const std::string &key, float                  val);
    void SetVal(const std::string &key, double                 val);
    void SetVal(const std::string &key, int                    val);
    void SetVal(const std::string &key, const std::string     &val);
    void SetVal(const std::string &key, const Anp::Registry   &val);
    void SetVal(const uint64_t     key, uint64_t               val);

    //
    // Access functions - T=string, Registry or number
    //
    template<class T> bool Get(const std::string &alg, const std::string& key, T &val) const;   
    template<class T> bool Get(const std::string &key,                         T &val) const;

    //
    // Set functions - T=string, char, Registry or number
    //
    template<class T> void Set(const std::string &key, const T &val);
    template<class T> void Set(const std::string &key, const T *val);

    //
    // String values are interpreted as comma separated list of numbers
    //
    template<class T> bool GetVec(const std::string &key,                         std::vector<T> &val) const;
    template<class T> bool GetVec(const std::string &alg, const std::string &key, std::vector<T> &val) const;
									    
    //
    // Functions for python to C++ translation via strings
    //
    void SetValueDouble(const std::string &key, const std::string &value);
    void SetValueLong  (const std::string &key, const std::string &value);

    //
    // Read key value as string and then convert it to vector using ',' delimeter
    //
    bool Get(const std::string &key, std::vector<std::string> &val) const;
    bool Get(const std::string &key, std::vector<int>         &val) const;
    bool Get(const std::string &key, std::vector<double>      &val) const;
    bool Get(const uint64_t     key, uint64_t                 &val) const;

    // Get internal storage vectors
    const std::vector<StrData>& GetStr() const { return fStr;    }
    const std::vector<DblData>& GetDbl() const { return fDbl;    }
    const std::vector<RegData>& GetReg() const { return fReg;    }
    const std::set   <U64Data>& GetU64() const { return fSetU64; }

    // Print and read methods
    void Print(std::ostream &os = std::cout,
	       unsigned int margin = 0, const std::string &key = "") const;

    void Read(std::istream &is) const;
    
    void PrintConfig(std::ostream &os = std::cout, const std::string &key = "") const;
    
    void WriteXML(const std::string  &path, unsigned left_pad = 0) const;
    void WriteXML(std::ostream &outf, unsigned left_pad = 0) const;

    void ReadXML(const std::string &path);

    static int StringToBool(const std::string &val);

  private:
    
    bool                  fUniqueKeys;
    std::vector<StrData>  fStr;
    std::vector<DblData>  fDbl;
    std::vector<RegData>  fReg;
    std::set<U64Data>     fSetU64;
  };

  //
  // XML parsing using lbxml2
  //
  Registry ParseXml(const std::string &fname, bool debug = false);

  //
  // Inlined functions
  //
  inline std::ostream& operator<<(std::ostream& os, const Anp::Registry &reg) 
  {
    reg.Print(os);
    return os;
  }
}

#include "PhysicsAnpBase/Registry.icc"
#endif
