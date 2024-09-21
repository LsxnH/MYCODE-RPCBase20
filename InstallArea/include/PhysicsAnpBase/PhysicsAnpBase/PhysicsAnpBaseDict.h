// -*- c++ -*-
#ifndef ANP_PHYSICSBASE_DICT_H
#define ANP_PHYSICSBASE_DICT_H

#include "PhysicsAnpBase/AlgEvent.h"
#include "PhysicsAnpBase/DataPair.h"
#include "PhysicsAnpBase/HistMan.h"
#include "PhysicsAnpBase/Registry.h"
#include "PhysicsAnpBase/ReadNtuple.h"
#include "PhysicsAnpBase/RunModule.h"
#include "PhysicsAnpBase/UtilBase.h"

#ifdef __GCCXML__

namespace Anp
{
  template void Registry::Set<int>        (const std::string &, const int &);
  template void Registry::Set<double>     (const std::string &, const double &);
  template void Registry::Set<std::string>(const std::string &, const std::string &);
  template void Registry::Set<Registry>   (const std::string &, const Registry &);
}

#endif
#endif
