// -*- c++ -*-
#ifndef ANP_UTILBASE_H
#define ANP_UTILBASE_H

// C/C++
#include <iostream>
#include <string>
#include <set>
#include <vector>

// ROOT
#include "TStopwatch.h"

class TDirectory;
class TH1;
class TTree;

namespace Anp
{
  class Registry;

  void StringTok(std::vector<std::string>& ls,
		 const std::string& str,
		 const std::string& tok);


  std::string ReplaceString(const std::string &value,
			    const std::string &oldstr,
			    const std::string &newstr);
  
  // Recursively make ROOT directories
  TDirectory* GetDir(std::string path, TDirectory *dir);
  TDirectory* GetDir(TDirectory *dir, std::string path);

  TDirectory* GetRecursiveDir(TDirectory *dir, std::string path);

  // Set histogram directory and name (if not empty)
  TH1* SetDir(TH1 *h, TDirectory *dir, const std::string &name = "");

  // Round variable using error as guidance
  std::pair<std::string, std::string> Round2Pair(double value, double error, int nprec=5);

  // Get file size in MB
  double file_size(const std::string &path);

  std::vector<int>         GetIntVec   (const std::string &list);
  std::vector<std::string> GetStringVec(const std::string &list);

  unsigned ReadVar(const Registry &reg,
		   const std::string &config,
		   const std::string &caller = "ReadVar");

  std::vector<unsigned> ReadVars(const Registry &reg,
				 const std::string &config,
				 const std::string &caller = "ReadVars");

  std::set<unsigned> ReadVarsAsSet(const Registry &reg,
				   const std::string &config,
				   const std::string &caller = "ReadVarsAsSet");

  uint32_t ReadCandId(const Registry &reg, const std::string &key);
  uint32_t ReadCandId(const Registry &reg, const std::string &key, std::string &value);

  bool HasBranch(TTree *tree, const std::string &branch, bool debug = false);

  //
  // Print stopwatch stat
  //
  std::string PrintResetStopWatch(TStopwatch &watch, bool do_reset=true);

  //
  // Helper class for timing events
  //
  class TimerScopeHelper
  {
  public:
    
    explicit TimerScopeHelper(TStopwatch &timer) 
      :fTimer(timer) { fTimer.Start(false); }
    ~TimerScopeHelper() { fTimer.Stop(); }
    
  private:
      
    TStopwatch &fTimer;
  };
}

//
// Templates
//
namespace Anp
{
  template <class T> void Print(const std::vector<T> &tvec, std::ostream &os) {
    if(tvec.empty()) return;
    for(unsigned int i = 0; i < tvec.size(); ++i) os << tvec[i] << " ";
    os << std::endl;
  }
}

#endif
