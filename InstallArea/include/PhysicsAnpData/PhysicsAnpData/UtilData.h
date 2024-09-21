// -*- c++ -*-
#ifndef ANP_UTILDATA_H
#define ANP_UTILDATA_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Author : Rustem Ospanov
 *
 * @Brief  : Utility functions for reading data classes
 * 
 **********************************************************************************/

// C/C++
#include <string>

namespace Anp
{
  //
  // Hash function (copied from TrigConfHLTData/src/HLTUtils.cxx)
  //
  uint32_t String2Hash(const std::string &s, const std::string &category = "");
  
  void PrintHashMap();

  std::string PadStrFront(std::string str, int width);
  std::string PadStrBack (std::string str, int width);

  void ThrowExceptionWithTrace(const std::string &msg);

  //
  // Compute barcode from algorithm name, object name and object position
  //
  uint64_t MakeBarcode(const std::string &alg, const std::string &name);
  uint64_t MakeBarcode(                        const std::string &name);
  uint64_t MakeBarcode();
}

#endif
