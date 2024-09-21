// -*- c++ -*-
#ifndef ANP_FOURMOM_H
#define ANP_FOURMOM_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : FourMom
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  FourMom is base class for Lorentz 4 vector interface
 *  
 **********************************************************************************/

// C/C++
#include <cmath>
#include <iostream>

// ROOT
#include "TLorentzVector.h"

namespace Anp
{
  class FourMom
  {   
  public:

    FourMom(); 
    virtual ~FourMom() {}
    
    void SetMomentum(double px, double py, double pz, double e);
    void AddMomentum(const FourMom &rhs);

    void SetE (double en) { fE  = en; } 
    void SetPx(double px) { fPx = px; } 
    void SetPy(double py) { fPy = py; } 
    void SetPz(double pz) { fPz = pz; } 

    void Scale  (double sf);
    void ScalePt(double sf);

    void SetEta      (double eta);
    void SetPtEtaPhiE(double pt, double eta, double phi, double e);
    void SetPtEtaPhiM(double pt, double eta, double phi, double m);
    
    double GetDR  (const FourMom &rhs) const;
    double GetDPhi(const FourMom &rhs) const;

    FourMom& operator+=(const FourMom &rhs);
    FourMom& operator-=(const FourMom &rhs);

    TLorentzVector GetLorentzVec() const { return TLorentzVector(fPx,fPy,fPz,fE); }
    TLorentzVector GetLV()         const { return TLorentzVector(fPx,fPy,fPz,fE); }

    double GetE () const { return fE;  }
    double GetPx() const { return fPx; }
    double GetPy() const { return fPy; }
    double GetPz() const { return fPz; }
    double GetP () const { return std::sqrt(fPx*fPx + fPy*fPy + fPz*fPz); }
    double GetP2() const { return           fPx*fPx + fPy*fPy + fPz*fPz;  }
    double GetPt() const { return std::sqrt(fPx*fPx + fPy*fPy);           }

    double GetM       () const;
    double GetM2      () const;
    double GetEt      () const;
    double GetCosTheta() const;
    double GetSinTheta() const;
    double GetEta     () const;
    double GetPhi     () const;

    double GetFracE        (const FourMom &rhs) const;
    double GetFracPt       (const FourMom &rhs) const;
    double GetDiffPt       (const FourMom &rhs) const;
    double GetDiffOverSumPt(const FourMom &rhs) const;

    //
    // Interface functions for derived classes
    //
    void SetFourMom(const FourMom &value)  { *this = value; }

    const FourMom& Get4Mom   ()  const { return *this; }
    const FourMom& GetFourMom()  const { return *this; }
    FourMom&       Get4Mom()           { return *this; }
    FourMom&       GetFourMom()        { return *this; }

    void ClearMom();

    void PrintMom(std::ostream &os = std::cout) const;

  private:

    double fPx;
    double fPy;
    double fPz;
    double fE;    
  };

  //
  // Inlined functions  
  //
  //======================================================================================================
  inline void FourMom::SetMomentum(double px, double py, double pz, double e) 
  {
    fPx = px; fPy = py; fPz = pz; fE = e;
  }

  //======================================================================================================
  inline void FourMom::AddMomentum(const FourMom &rhs) 
  {
    fPx += rhs.GetPx(); fPy += rhs.GetPy(); fPz += rhs.GetPz(); fE += rhs.GetE();
  }
  
  //======================================================================================================
  inline double FourMom::GetDPhi(const FourMom &rhs) const
  {
    return TVector2::Phi_mpi_pi(GetPhi() - rhs.GetPhi()); 
  }
  
  //======================================================================================================
  inline double FourMom::GetDR(const FourMom &rhs) const
  {
    //
    // Get DR between two vectors
    //
    const double deta = GetEta() - rhs.GetEta();
    const double dphi = GetDPhi(rhs);
  
    return std::sqrt(deta*deta + dphi*dphi);
  }

  //======================================================================================================
  inline double FourMom::GetEta() const
  {
    //
    // Return PseudoRapidity
    //
    
    const double cosTheta = GetCosTheta();
    
    if(cosTheta*cosTheta < 1) { 
      return -0.5* std::log( (1.0-cosTheta)/(1.0+cosTheta) );
    }
    
    if(fPz > 0) return  10e10;
    else        return -10e10;
    
    return 0.0;
  }
  
  //======================================================================================================
  inline double FourMom::GetCosTheta() const
  {
    //
    // Return cos(theta) angle
    //
    const double p = GetP();
    
    if(p > 0.0) {
      return fPz/p;
    }
    
  return 0.0;
  }
  
  //======================================================================================================
  inline double FourMom::GetSinTheta() const
  {
    //
    // Return sin(theta) angle
    //
    const double p = GetP();
    
    if(p > 0.0) {
      return GetPt()/p;
    }
    
    return 0.0;
  }
  
  //======================================================================================================
  inline FourMom operator+(const FourMom &lhs, const FourMom &rhs)
  {
    FourMom m(lhs);
    m += rhs;
    return m;
  }

  //======================================================================================================
  inline FourMom operator-(const FourMom &lhs, const FourMom &rhs)
  {
    FourMom m(lhs);
    m -= rhs;
    return m;
  }
}

#endif
