#ifndef ANP_RECVERTEX_H
#define ANP_RECVERTEX_H

// C/C++
#include <algorithm>
#include <cmath>

// Local
#include "PhysicsAnpData/VarHolder.h"

// ROOT
#include "TVector3.h"

namespace Anp
{
  namespace Vertex 
  {

    enum Type {           // Copied from TrkEventPrimitives/VertexType.h  
      NoVtx   = 0,        // Dummy vertex, TrackParticle was not used in vertex fit
      PriVtx  = 1,        // Primary Vertex
      SecVtx  = 2,        // Secondary Vertex
      PileUp  = 3,        // Pile Up Vertex
      ConvVtx = 4,        // Converstion Vertex
      V0Vtx   = 5,        // Vertex from V0 Decay
      KinkVtx = 6,        // Kink Vertex
      V0Lambda = 7,       // Temporary addition for V0 Lambda
      V0LambdaBar = 8,    // Temporary addition for V0 LambdaBar
      V0KShort = 9,       // Temporary addition for KShort
      NotSpecified = -99, // this is the default
      NONE,               // Vtx type is not set
      AnyVtx              // Any valid type - used for plotting
    };

    Type        Convert2Type  (const std::string &type);
    std::string Convert2String(const Type type);
  }

  class RecVertex: public VarHolder
  {    
  public:

    RecVertex(); 
    virtual ~RecVertex() {}

    static std::string GetObjectType() { return "RecVertex"; }
    
    void SetType(Vertex::Type t) { fVtxType = t;}

    void SetNTracks(unsigned n)    { fNTracks = n;    }
    void SetIndex  (int      n)    { fIndex   = n;    }
    void SetFitChi2(float    chi2) { fFitChi2 = chi2; }
    void SetFitNdof(unsigned ndof) { fFitNdof = ndof; }
    void SetBarcode(uint64_t bc)   { fBarcode = bc;   }

    void SetSumPt (float s) { fSumPt  = s; }
    void SetSumPt2(float s) { fSumPt2 = s; }
    void SetX     (float x) { fX      = x; }
    void SetY     (float y) { fY      = y; }
    void SetZ     (float z) { fZ      = z; }
    void SetZErr  (float z) { fZErr   = z; }

    Vertex::Type GetType() const { return fVtxType; }

    unsigned GetNTracks() const { return fNTracks; }
    int      GetIndex  () const { return fIndex;   }    
    float    GetFitChi2() const { return fFitChi2; }
    unsigned GetFitNdof() const { return fFitNdof; }
    uint64_t GetBarcode() const { return fBarcode; }

    float GetX     () const { return fX;      }
    float GetY     () const { return fY;      }
    float GetZ     () const { return fZ;      }
    float GetZErr  () const { return fZErr;   }
    float GetSumPt () const { return fSumPt;  }
    float GetSumPt2() const { return fSumPt2; }

    TVector3 GetVect() const { return TVector3(fX,fY,fZ); }
    
    void Clear();
    
    void Dump() const;
       
  private:

    Vertex::Type   fVtxType;

    int16_t        fIndex;
    uint16_t       fNTracks;    
    uint16_t       fFitNdof;
    float          fFitChi2;

    float          fX;
    float          fY;
    float          fZ;
    float          fZErr;

    float          fSumPt;
    float          fSumPt2;

    uint64_t       fBarcode;       // transient value - not saved in ntuples
  };

  //
  // Inline global functions
  //
  inline bool operator==(const RecVertex &vertex, const int index) {
    return vertex.GetIndex() == index;
  }
  inline bool operator==(const int index, const RecVertex &vertex) {
    return vertex.GetIndex() == index;
  }

  inline bool operator<(const RecVertex &vertex, const int index) {
    return vertex.GetIndex() < index;
  }
  inline bool operator<(const int index, const RecVertex &vertex) {
    return index < vertex.GetIndex();
  }
}

#endif
