// This is -*- c++ -*-
#ifndef ANP_RECTRACK_H
#define ANP_RECTRACK_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecTrack
 * @Author : Rustem Ospanov
 *
 * This class implements light version of ATLAS Rec::TrackParticle class:
 * 
 * https: *svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Particle/trunk/Particle/TrackParticle.h
 *
 * Following variables are always filled:
 *  - track type
 *  - track 4-momentum vector
 *  - fit chi2 and ndof
 *  - number of hits in various tracking subdetectors
 *  - index for matching primary or pileup vertex
 *
 *  Following variables are optional - do not assume that ther are filled!
 *
 *  - Track measurements and errors at various surfaces/points
 *    measurement type is enumerated by MeasDef.
 *    Only one measurement of each type is allowed.
 *
 *  - Individual track hits in various subdetectors
 *  
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <cmath>
#include <vector>

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/RecHit.h"
#include "PhysicsAnpData/VarHolder.h"

namespace Anp
{
  namespace Track
  {
    enum Type {
      NoType         = 0,  // Track type is not set
      IDTrack        = 1,  // Inner detector track
      MSTrack        = 2,  // Muon spectrometer track
      CombinedMuon   = 3,  // Combined muon track
      IDExtrapolated = 4,  // Inner detector extapolated track
      MSExtrapolated = 5   // Muon spectrometer extrapolated track
    };
    
    enum MeasDef {
      PriVtx     = 1,  // Track parameter measurements at primary   vertex
      TrkVtx     = 2,  // Track parameter measurements at secondary vertex
      PileUp     = 3,  // Track parameter measurements at pileup    vertex (exact vertex not defined)
      Perigee    = 4,  // Track parameter measurements at perigee
      Beamspot   = 5,  // Track parameter measurements at beamspot
      ExtrapID   = 6,  // Track parameter measurements extrapolated to ID 
      ExtrapMS   = 7,  // Track parameter measurements extrapolated to MS
      UnbiasedPV = 8,  // Unbiased Track parameter measurements at primary vertex
      NoDef          // Parameters are not filled
    };

    enum Pattern {
      
     SiSPSeededFinder             = 0x1,       /** Tracks from SiSPSeedFinder*/
     SiCTBTracking                = 0x2,       /** Tracks from SiCTBTracking */         
     InDetAmbiguitySolver         = 0x4,       /** Tracks with InDetAmbiguitySolver used*/            
     InDetExtensionProcessor      = 0x8,       /** Tracks with InDetExtensionProcessor used*/ 
     TRTSeededTrackFinder         = 0x10,      /** Tracks from TRT Seeded Track finder*/       
     Muonboy                      = 0x20,      /** Tracks from MuonBoy*/     
     MuGirlUnrefitted             = 0x40,      /** Tracks from MuGirl not refitted with iPat*/
     STACO                        = 0x80,      /** Tracks produced by STACO*/
     StacoLowPt                   = 0x100,     /** Tracks produced by StacoLowpt*/
     MuTag                        = 0x200,     /** Tracks produced by MuTag*/	
     MooreToTrackTool             = 0x400,     /** Tracks produced by Moore */
     TRTStandalone                = 0x800,     /** TRT Standalone */	  
     MuidStandAlone               = 0x1000,    /** MuidStandalone */     
     TRTSeededSingleSpTrackFinder = 0x2000,    /** TRTSeededSingleSpTrackFinder */
     Moore                        = 0x4000,    /** Moore (MoMu) */ 
     MuidCombined	          = 0x8000,    /** MuidCombined */     
     MuidVertexAssociator         = 0x10000,   /** MuidVertexAssociator */     
     MuGirl                       = 0x20000,   /** MuGirl */     
     iPatRec                      = 0x40000,   /** iPatRec */     
     MuGirlLowBeta                = 0x80000,   /** MuGirlLowBeta */     
     ReverseOrderedTrack          = 0x100000,  /** Track which has its hits behind it */
     MuidMuonRecoveryTool         = 0x200000,  /** Muons found by the ID seeded muon recovery */
     MuidStandaloneRefit          = 0x400000   /** Standalone muon that was obtained by refitting a combined muon */
    };

    Type        Convert2Type  (const std::string &type);
    std::string Convert2String(const Type type);

    MeasDef     Convert2MeasDef(const std::string &def);
    std::string Convert2String (const MeasDef def);
  }

  class TrackPar {
  public:
    
    TrackPar(Track::MeasDef def = Track::NoDef); 
    ~TrackPar() {} 

    void SetD0    (float d0)     { fd0     = d0;     }
    void SetZ0    (float z0)     { fz0     = z0;     }
    void SetPhi   (float phi)    { fphi0   = phi;    }
    void SetTheta (float theta)  { ftheta  = theta;  }
    void SetQoverP(float qOverP) { fqOverP = qOverP; }

    float GetD0()       const { return fd0;                 }
    float GetZ0()       const { return fz0;                 }
    float GetTheta()    const { return ftheta;              }
    float GetSinTheta() const { return std::sin(ftheta);    }
    float GetPhi()      const { return fphi0;               }
    float GetQoverP()   const { return fqOverP;             }
    float GetEta()      const { return -1*std::log(std::tan(ftheta/2)); }
    float GetPt()       const { return fabs(1.0/GetQoverP()*std::sin(GetTheta())); }
    Track::MeasDef GetDef() const { return fDef; }

  private:
    
    Track::MeasDef fDef;         // Measurement definition
    float          fd0;          // Trk::d0
    float          fz0;          // Trk::z0
    float          fphi0;        // Trk::phi0
    float          ftheta;       // Trk::theta
    float          fqOverP;      // Trk::qOverP
  };

  class TrackErr {
  public:
    
    TrackErr(Track::MeasDef def = Track::NoDef); 
    ~TrackErr() {} 
    
    void SetD0Err    (float d0)     { fd0Err     = d0;     }
    void SetZ0Err    (float z0)     { fz0Err     = z0;     }
    void SetPhiErr   (float phi)    { fphi0Err   = phi;    }
    void SetThetaErr (float theta)  { fthetaErr  = theta;  }
    void SetQoverPErr(float qOverP) { fqOverPErr = qOverP; }

    float GetD0Err()     const { return fd0Err;     }
    float GetZ0Err()     const { return fz0Err;     }
    float GetThetaErr()  const { return fthetaErr;  }
    float GetPhiErr()    const { return fphi0Err;   }
    float GetQoverPErr() const { return fqOverPErr; }
    float GetEtaErr()    const { return -1*std::log(std::tan(fthetaErr/2)); }
    
    Track::MeasDef GetDef() const { return fDef; }
    
  private:

    Track::MeasDef fDef;         // Measurement definition
    float          fd0Err;       // Error for Trk::d0
    float          fz0Err;       // Error for Trk::z0
    float          fphi0Err;     // Error for Trk::phi0
    float          fthetaErr;    // Error for Trk::theta
    float          fqOverPErr;   // Error for Trk::qOverP
  };

  class RecTrack: public VarHolder
  {
  public:

    enum Bits {
      NoBit        = 0x0,
      ExpectBLayer = 0x1,
      IsSelect     = 0x2,
      IsMatch      = 0x4,
      IsMatchLep   = 0x8
    };

    enum Key {
      EtCone20      = 1000,
      EtCone30      = 1001,
      EtCone40      = 1002,
      PtCone20      = 1003,
      PtCone30      = 1004,
      PtCone40      = 1005,
      EtCone10      = 1006,
      PtCone10      = 1007,
      UnbiasedD0    = 1008,
      UnbiasedZ0    = 1009,
      UnbiasedD0Err = 1010,
      PixeldEdx     = 2000
    };

  public:

    RecTrack(Track::Type type = Track::NoType); 
    virtual ~RecTrack() {}
    
    static std::string GetObjectType() { return "RecTrack"; }

    void SetNBLayer        (int n) { if(Check8bit(n)) fnBLayer         = n; }
    void SetNBLayerOutliers(int n) { if(Check8bit(n)) fnBLayerOutliers = n; }
    void SetNPixel         (int n) { if(Check8bit(n)) fnPixel          = n; }
    void SetNPixelOutliers (int n) { if(Check8bit(n)) fnPixelOutliers  = n; }
    void SetNPixelHoles    (int n) { if(Check8bit(n)) fnPixelHoles     = n; }
    void SetNPixelDead     (int n) { if(Check8bit(n)) fnPixelDead      = n; }
    void SetNSCT           (int n) { if(Check8bit(n)) fnSCT            = n; }
    void SetNSCTOutliers   (int n) { if(Check8bit(n)) fnSCTOutliers    = n; }
    void SetNSCTHoles      (int n) { if(Check8bit(n)) fnSCTHoles       = n; }
    void SetNSCTDead       (int n) { if(Check8bit(n)) fnSCTDead        = n; }
    void SetNTRT           (int n) { if(Check8bit(n)) fnTRT            = n; }
    void SetNTRTOutliers   (int n) { if(Check8bit(n)) fnTRTOutliers    = n; }
    void SetNTRTHL         (int n) { if(Check8bit(n)) fnTRTHL          = n; }
    void SetNTRTHLOutliers (int n) { if(Check8bit(n)) fnTRTHLOutliers  = n; }
    void SetNMDT           (int n) { if(Check8bit(n)) fnMDT            = n; }
    void SetNTGCPhi        (int n) { if(Check8bit(n)) fnTGCPhi         = n; }
    void SetNTGCEta        (int n) { if(Check8bit(n)) fnTGCEta         = n; }
    void SetNCSCPhi        (int n) { if(Check8bit(n)) fnCSCPhi         = n; }
    void SetNCSCEta        (int n) { if(Check8bit(n)) fnCSCEta         = n; }
    void SetNRPCPhi        (int n) { if(Check8bit(n)) fnRPCPhi         = n; }
    void SetNRPCEta        (int n) { if(Check8bit(n)) fnRPCEta         = n; }

    void SetBarcode       (uint64_t b) { fBarcode        = b; }
    void SetVertexIndex   (int      i) { fVertexIndex    = i; }
    void SetFitNdof       (int      n) { fFitNdof        = n; }
    void SetFitChi2       (float    c) { fFitChi2        = c; }
    void SetExpectBLayer  (bool     n) { if(n) AddTrackBit(ExpectBLayer); }
    
    void SetType(Track::Type type) { fType = type; }

    uint32_t GetBitsField() const        { return fTrackBits;        }
    void     SetBitsField(uint32_t bits) { fTrackBits = bits & 0xff; }

    void AddTrackBit(Bits           bit) { AddBit(bit);                       }
    void AddBit     (Bits           bit) { fTrackBits |= bit;                 }
    void ResetBit   (Bits           bit) { fTrackBits  = fTrackBits & (~bit); }
    void AddPattern (Track::Pattern bit) { fPattern   |= bit;                 }

    void SetEtCone10(float iso) { AddVar(EtCone10, iso); }
    void SetEtCone20(float iso) { AddVar(EtCone20, iso); }
    void SetEtCone30(float iso) { AddVar(EtCone30, iso); }
    void SetEtCone40(float iso) { AddVar(EtCone40, iso); }

    void SetPtCone10(float iso) { AddVar(PtCone10, iso); }
    void SetPtCone20(float iso) { AddVar(PtCone20, iso); }
    void SetPtCone30(float iso) { AddVar(PtCone30, iso); }
    void SetPtCone40(float iso) { AddVar(PtCone40, iso); }

    bool AddPar (const TrackPar &par);
    bool AddErr (const TrackErr &err);
    void AddHit (const RecHit   &hit) { fHits.push_back(hit);  }         
    
    unsigned GetNSiHit         () const;
    unsigned GetNBLayerOutliers() const { return fnBLayerOutliers; }
    unsigned GetNBLayer()         const { return fnBLayer;         }
    unsigned GetNPixel()          const { return fnPixel;          }
    unsigned GetNPixelOutliers()  const { return fnPixelOutliers;  }
    unsigned GetNPixelHoles()     const { return fnPixelHoles;     }
    unsigned GetNPixelDead()      const { return fnPixelDead;      }
    unsigned GetNSCT()            const { return fnSCT;            }
    unsigned GetNSCTOutliers()    const { return fnSCTOutliers;    }
    unsigned GetNSCTHoles()       const { return fnSCTHoles;       }
    unsigned GetNSCTDead()        const { return fnSCTDead;        }
    unsigned GetNTRT()            const { return fnTRT;            }
    unsigned GetNTRTOutliers()    const { return fnTRTOutliers;    }
    unsigned GetNTRTHL()          const { return fnTRTHL;          }
    unsigned GetNTRTHLOutliers()  const { return fnTRTHLOutliers;  }
    unsigned GetNMDT()            const { return fnMDT;            }
    unsigned GetNTGCPhi()         const { return fnTGCPhi;         }
    unsigned GetNTGCEta()         const { return fnTGCEta;         }
    unsigned GetNCSCPhi()         const { return fnCSCPhi;         }
    unsigned GetNCSCEta()         const { return fnCSCEta;         }
    unsigned GetNRPCPhi()         const { return fnRPCPhi;         }
    unsigned GetNRPCEta()         const { return fnRPCEta;         }
    uint64_t GetBarcode()         const { return fBarcode;         }

    int   GetVertexIndex()  const { return fVertexIndex; }
    int   GetFitNdof()      const { return fFitNdof;     }
    float GetFitChi2()      const { return fFitChi2;     }

    double GetTRTOLRatio()   const;
    double GetTRTHLRatio()   const;

    bool  GetExpectBLayer() const { return fTrackBits & ExpectBLayer;        }
    float GetD0wrtVtx()     const { return GetPar(Track::PriVtx).GetD0();    }
    float GetD0errwrtVtx()  const { return GetErr(Track::PriVtx).GetD0Err(); }
    float GetZ0wrtVtx()     const { return GetPar(Track::PriVtx).GetZ0();    }
    float GetZ0errwrtVtx()  const { return GetErr(Track::PriVtx).GetZ0Err(); }

    float GetD0()          const { return GetPar(Track::Perigee).GetD0();        }
    float GetD0err()       const { return GetErr(Track::Perigee).GetD0Err();     }
    float GetZ0()          const { return GetPar(Track::Perigee).GetZ0();        }
    float GetZ0err()       const { return GetErr(Track::Perigee).GetZ0Err();     }
    float GetEta()         const { return GetPar(Track::Perigee).GetEta();       }
    float GetTheta()       const { return GetPar(Track::Perigee).GetTheta();     }
    float GetThetaErr()    const { return GetErr(Track::Perigee).GetThetaErr();  }
    float GetEtaerr()      const { return GetErr(Track::Perigee).GetEtaErr();    }
    float GetPhi()         const { return GetPar(Track::Perigee).GetPhi();       }
    float GetPhierr()      const { return GetErr(Track::Perigee).GetPhiErr();    }    
    float GetQoverP()      const { return GetPar(Track::Perigee).GetQoverP();    }
    float GetQoverPerr()   const { return GetErr(Track::Perigee).GetQoverPErr(); }
    
    Track::Type GetType() const { return fType; }

    bool HasPar(const Track::MeasDef type) const;
    bool HasErr(const Track::MeasDef type) const;

    const TrackPar& GetPar(const Track::MeasDef type) const;
    const TrackErr& GetErr(const Track::MeasDef type) const;

    std::vector<RecHit>&  GetHits()     { return fHits;     }
    FourMom&              GetFourMom()  { return fFourMom;  }

    const std::vector<TrackPar>& GetTrackPar() const { return fPars; }
    const std::vector<TrackErr>& GetTrackErr() const { return fErrs; }
    const std::vector<RecHit>&   GetHits()     const { return fHits; }
    
    const FourMom&     Get4Mom()     const { return fFourMom;  }
    const FourMom&     GetFourMom()  const { return fFourMom;  }

    bool HasBit    (Bits           bit) const { return fTrackBits & bit; }
    bool HasPattern(Track::Pattern bit) const { return fPattern   & bit; }

    void Clear();
    void Print(std::ostream &os = std::cout) const;

  private:
    
    bool Check8bit(int) const; 
    
  private:

    Track::Type                fType;
    uint8_t                    fTrackBits;
    uint32_t                   fPattern;

    uint8_t                    fnMDT;
    uint8_t                    fnTGCPhi;
    uint8_t                    fnTGCEta;
    uint8_t                    fnCSCPhi;
    uint8_t                    fnCSCEta;
    uint8_t                    fnRPCPhi;
    uint8_t                    fnRPCEta;

    uint8_t                    fnBLayer;
    uint8_t                    fnBLayerOutliers; 
    uint8_t                    fnPixel;
    uint8_t                    fnPixelOutliers;
    uint8_t		       fnPixelHoles;
    uint8_t		       fnPixelDead;
    uint8_t                    fnSCT;
    uint8_t                    fnSCTOutliers;
    uint8_t		       fnSCTHoles;
    uint8_t		       fnSCTDead;
    uint8_t                    fnTRT;
    uint8_t                    fnTRTOutliers;
    uint8_t                    fnTRTHL;
    uint8_t                    fnTRTHLOutliers;

    uint64_t                   fBarcode;          // Athena barcode
    int16_t                    fVertexIndex;      // Index for associated vertex - if exists!
    int16_t                    fFitNdof;          // Track fit ndof          
    float                      fFitChi2;          // Track fit chi2

    std::vector<Anp::TrackPar> fPars;             // Track parameters
    std::vector<Anp::TrackErr> fErrs;             // Track parameter errors
    std::vector<Anp::RecHit>   fHits;             // Detector hits

    FourMom                    fFourMom;          // Basic Four Momentum       
  };

  //
  // Inlined functions
  //
  inline unsigned RecTrack::GetNSiHit() const
    {
      return GetNPixel()+GetNPixelOutliers()+GetNSCT()+GetNSCTOutliers();
    }
}

#endif
