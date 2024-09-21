// -*- c++ -*-
#ifndef ANP_TRUTHPART_H
#define ANP_TRUTHPART_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : TruthPart
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  MC truth particle
 * 
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <stdint.h>
#include <vector>

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"
#include "PhysicsAnpData/Ptr.h"

namespace Anp
{
  std::string AsStringTruthPdgId(int orig);

  bool IsLightQuark(int pdgid);
  bool IsBQuark    (int pdgid);
  bool IsGluon     (int pdgid);


  class TruthPart: public ObjectBase, public FourMom
  {    
  public:

    TruthPart(); 
    virtual ~TruthPart() {}

    static std::string GetObjectType() { return "TruthPart"; }

    void SetPdgId      (int i) { fPdgId       = i; }
    void SetStatus     (int s) { fStatus      = s; }
    void SetTrueBarcode(int b) { fTrueBarcode = b; }

    void AddChild (const Ptr<TruthPart> &ptr);
    void AddParent(const Ptr<TruthPart> &ptr);

    const std::vector<Ptr<TruthPart> >& GetChildren() const { return fChildren; }
    const std::vector<Ptr<TruthPart> >& GetParents () const { return fParents;  }

    std::vector<Ptr<TruthPart> >& GetChildren() { return fChildren; }
    std::vector<Ptr<TruthPart> >& GetParents () { return fParents;  }
    
    bool IsChild(const TruthPart &tp) const;

    int  GetPdgId      () const { return fPdgId;           }
    int  GetPdgIdAbs   () const { return std::abs(fPdgId); }
    int  GetStatus     () const { return fStatus;          } 
    int  GetTrueBarcode() const { return fTrueBarcode;     }
    bool IsGenerator   () const;
    
    void Clear();

    std::string AsStr(const std::string &pad = "") const;

    void Print(std::ostream &os = std::cout) const { os << AsStr() << std::endl; }
    
    std::string AsStrParents (const std::string &pad = "") const;
    std::string AsStrChildren(const std::string &pad = "") const;

  private:


  private:

    int                            fPdgId;       // Particle PDG id
    int                            fStatus;      // Particle status
    int                            fTrueBarcode; // Particle true barcode - from HepMC::GenParticle::barcode()

    std::vector<float>             fVertex;      // TODO
    std::vector<Ptr<TruthPart> >   fChildren;    // Children of this truth particle
    std::vector<Ptr<TruthPart> >   fParents;     // Parents  of this truth particle
  };
}

#endif
