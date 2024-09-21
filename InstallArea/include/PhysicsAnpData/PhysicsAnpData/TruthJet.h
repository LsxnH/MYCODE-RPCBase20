// -*- c++ -*-
#ifndef ANP_TRUTHJET_H
#define ANP_TRUTHJET_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecJet
 * @Author : Rustem Ospanov
 *
 * @Brief  : Truth jet class
 * 
 **********************************************************************************/

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/ObjectBase.h"
#include "PhysicsAnpData/TruthPart.h"

namespace Anp
{
  class TruthJet: public ObjectBase, public FourMom
  {    
  public:

    TruthJet(); 
    virtual ~TruthJet() {}
    
    static std::string GetObjectType() { return "TruthJet"; }

    void AddConstituent(const Ptr<TruthPart> &ptr);
    void AddGhostParton(const Ptr<TruthPart> &ptr);

    const std::vector<Ptr<TruthPart> >& GetConstituents() const { return fConstituents; }
    const std::vector<Ptr<TruthPart> >& GetGhostPartons() const { return fGhostPartons;  }

    std::vector<Ptr<TruthPart> >& GetConstituents() { return fConstituents; }
    std::vector<Ptr<TruthPart> >& GetGhostPartons() { return fGhostPartons; }
    
    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:
    
    std::vector<Ptr<TruthPart> >   fConstituents;
    std::vector<Ptr<TruthPart> >   fGhostPartons;
  };
}

#endif
