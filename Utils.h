// -*- c++ -*-
#ifndef ANP_PHYSICSANPRPC_UTILS_H
#define ANP_PHYSICSANPRPC_UTILS_H

// C/C++
#include <iostream>
#include <climits>

// Base
#include "PhysicsAnpData/VarHolder.h"

// Local
#include "RpcGeo.h"
#include "VarDefs.h"

class TH1;
class TH2;
class TDirectory;

namespace Rpc
{
  TH1* MakeStripHist(const Anp::Ptr<Anp::RpcGeoGap> &geo_gap, 
		     const Anp::Geo::View view,
		     TDirectory *dir,
		     const std::string &hname,
		     const std::string &xtitle,
		     const std::string &ytitle);

  TH2* MakeStripHist2d(const Anp::Ptr<Anp::RpcGeoGap> &geo_gap, 
		       TDirectory *dir,
		       const std::string &hname,
		       const std::string &xtitle,
		       const std::string &ytitle);

  unsigned GetRpcTriggerSector(double eta, double phi);

  template<typename T> void ReadVar(const Anp::VarHolder &h, Anp::Var::Def var, T &val)
  {
    if(!h.GetVar(var, val)) {
      if(false) std::cout << "Rpc::ReadVar - missing variable: " << Anp::Var::AsStr(var) << std::endl;
    }
  }

  template<> inline void ReadVar<unsigned short>(const Anp::VarHolder &h, Anp::Var::Def var, unsigned short &val)
  {
    double tmp = 0.0;

    if(!h.GetVar(var, tmp)) {
      if(false) std::cout << "Rpc::ReadVar - missing variable: " << Anp::Var::AsStr(var) << std::endl;
    }
    else {
      if(tmp < 0.0 || tmp > USHRT_MAX) {
	std::cout << "Rpc::ReadVar - unsigned short with bad value: " << Anp::Var::AsStr(var) << " = " << tmp << std::endl; 
      }
      else {
	val = static_cast<unsigned short>(tmp);
      }
    }
  }

  template<> inline void ReadVar<unsigned int>(const Anp::VarHolder &h, Anp::Var::Def var, unsigned int &val)
  {
    double tmp = 0.0;

    if(!h.GetVar(var, tmp)) {
      if(false) std::cout << "Rpc::ReadVar - missing variable: " << Anp::Var::AsStr(var) << std::endl;
    }
    else {
      if(tmp < 0.0 || tmp > UINT_MAX) {
	std::cout << "Rpc::ReadVar - unsigned variable has negative value: " << Anp::Var::AsStr(var) << " = " << tmp << std::endl; 
      }
      else {
	val = static_cast<unsigned int>(tmp);
      }
    }
  }

  template<> inline void ReadVar<int>(const Anp::VarHolder &h, Anp::Var::Def var, int &val)
  {
    double tmp = 0.0;

    if(!h.GetVar(var, tmp)) {
      if(false) std::cout << "Rpc::ReadVar - missing variable: " << Anp::Var::AsStr(var) << std::endl;
    }
    else {
      val = static_cast<int>(tmp);

      if(tmp != double(val)) {
	std::cout << "Rpc::ReadVar - lost precision in double to int conversion: " << Anp::Var::AsStr(var) << " = " << tmp << std::endl; 
      }
    }
  }
  
  template<> inline void ReadVar<ULong64_t>(const Anp::VarHolder &h, Anp::Var::Def var, ULong64_t &val)
  {
    if(!h.GetVarU64(var, val)) {
      if(false) std::cout << "Rpc::ReadVar - missing variable: " << Anp::Var::AsStr(var) << std::endl;
    }
  }
}

#endif
