// This is -*- c++ -*-
#ifndef ANP_INPUTINFO_H
#define ANP_INPUTINFO_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : InputInfo
 * @Author : Rustem Ospanov
 *
 * @Brief  : Info for input file and tree
 * 
 **********************************************************************************/

// C/C++
#include <string>
#include <utility>

namespace Anp
{
  class InputInfo
  {
  public:

    InputInfo();
    ~InputInfo() {}
    
    void SetTreeEntry(long               entry) { fTreeEntry = entry; }
    void SetFilePath (const std::string &path);
    void SetTreeName (const std::string &tree);    

    double             GetTreeEntry() const { return fTreeEntry; }
    const std::string& GetTreeName () const { return fTreeName;  }
    const std::string& GetFilePath () const { return fFilePath;  }

    size_t                    GetFileHash () const { return fFileHash;  }
    size_t                    GetTreeHash () const { return fTreeHash;  }
    std::pair<size_t, size_t> GetHashPair () const;

    bool IsSameFile(const InputInfo &other) const;
    bool IsSameTree(const InputInfo &other) const;    

  private:

    std::string    fFilePath;
    std::string    fTreeName;

    long           fTreeEntry;

    size_t         fFileHash;
    size_t         fTreeHash;
  };

  //==============================================================================
  // Comparison operators
  //==============================================================================
  inline bool operator==(const InputInfo &lhs, const InputInfo &rhs)
  {
    return lhs.GetHashPair() == rhs.GetHashPair();
  }

  //==============================================================================
  inline bool operator <(const InputInfo &lhs, const InputInfo &rhs)
  {
    return lhs.GetHashPair() < rhs.GetHashPair();
  }
}

#endif
