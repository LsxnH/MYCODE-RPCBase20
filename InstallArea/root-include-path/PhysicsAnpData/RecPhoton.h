// -*- c++ -*-
#ifndef ANP_RECPHOTON_H
#define ANP_RECPHOTON_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecElec
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Reconstructed photon class
 * 
 **********************************************************************************/

#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class RecPhoton: public ObjectBase
  {
  public:

    RecPhoton(); 
    virtual ~RecPhoton() {}
    
    static std::string GetObjectType() { return "RecPhoton"; }

    void SetIsem      (int      isem) { fIsem          = isem; }
    void SetAuthor    (int      auth) { fAuthor        = auth; }
    void SetBarcode   (uint64_t bc)   { fAthenaBarcode = bc;   }

    uint64_t GetBarcode   () const { return fAthenaBarcode; }
    int      GetAuthor    () const { return fAuthor;        }
    int      GetIsem      () const { return fIsem;          }

    bool GetIsem  (unsigned mask) const { return fIsem  & mask; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:

    uint64_t    fAthenaBarcode;  

    int         fIsem;
    int         fAuthor;
    uint32_t    fPhysicsBits;
  };    
}

#endif
