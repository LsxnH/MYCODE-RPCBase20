// -*- c++ -*-
#ifndef ANP_TRUTHVTX_H
#define ANP_TRUTHVTX_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : TruthVtx
 * @Author : Rustem Ospanov
 *
 * @Brief  : MC truth vertex information
 * 
 **********************************************************************************/

// C/C++
#include <vector>

// Local
#include "PhysicsAnpData/ObjectBase.h"

namespace Anp
{
  class TruthVtx: public ObjectBase
  {    
  public:

    enum Bits {
      NoBit        = 0,
      Signal       = 0x1,
      InTimePileup = 0x2,
      Dummy        = 0x4
    };

    TruthVtx(); 
    virtual ~TruthVtx() {}

    static std::string GetObjectType() { return "TruthVtx"; }

    void SetTrueBarcode(int v) { fTrueBarcode = v; }
    void SetIndex      (int v) { fIndex       = v; }

    void AddInpParticle(int barcode);
    void AddOutParticle(int barcode);

    void SetVertex(double vx, double vy, double vz, double vt);
    void SetX     (double x) { fVtxX = x; }
    void SetY     (double y) { fVtxY = y; }
    void SetZ     (double z) { fVtxZ = z; }
    void SetT     (double t) { fVtxT = t; }

    void AddBit(Bits bit)       {        fBits |= bit; }
    bool HasBit(Bits bit) const { return fBits  & bit; }

    int    GetTrueBarcode() const { return fTrueBarcode; }
    int    GetIndex      () const { return fIndex;       }    
    double GetVtxX       () const { return fVtxX;        }
    double GetVtxY       () const { return fVtxY;        }
    double GetVtxZ       () const { return fVtxZ;        }
    double GetVtxT       () const { return fVtxT;        }
    double GetVtxR       () const { return std::sqrt(fVtxX*fVtxX + fVtxY*fVtxY); }

    bool IsInpParticle(int barcode) const;
    bool IsOutParticle(int barcode) const;

    bool MatchInpParticle(const std::vector<int> &barcodes) const;
    bool MatchOutParticle(const std::vector<int> &barcodes) const;

    unsigned GetNInp() const { return fInpParticles.size(); }
    unsigned GetNOut() const { return fOutParticles.size(); }

    const std::vector<int32_t>& GetInpParticles() const { return fInpParticles; }
    const std::vector<int32_t>& GetOutParticles() const { return fOutParticles; }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:
    
    int32_t      fTrueBarcode;  // Vertex barcode
    int16_t      fIndex;        // Vertex index
    uint32_t     fBits;         // Bits defined locally

    double       fVtxX;         // Vertex X position
    double       fVtxY;         // Vertex Y position
    double       fVtxZ;         // Vertex Z position
    double       fVtxT;         // Vertex time

    std::vector<int32_t>    fInpParticles;     // Barcodes of input    particles
    std::vector<int32_t>    fOutParticles;     // Barcodes of produced particles
  };
}

#endif
