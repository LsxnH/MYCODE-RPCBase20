// -*- c++ -*-
#ifndef ANP_RECCLUSTER_H
#define ANP_RECCLUSTER_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : RecCluster
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Reconstructed cluster class
 * 
 **********************************************************************************/

// C/C++
#include <vector>
#include <cmath>
#include <algorithm>

// Local
#include "PhysicsAnpData/FourMom.h"
#include "PhysicsAnpData/VarHolder.h"

namespace Anp
{  
  class RecCluster: public VarHolder
  {
  public:

    RecCluster(); 
    virtual ~RecCluster() {}

    static std::string GetObjectType() { return "RecCluster"; }

    void SetClusterE  (float clusterE)   { fClusterE   = clusterE;   }
    void SetClusterPhi(float clusterPhi) { fClusterPhi = clusterPhi; }
    void SetClusterEta(float clusterEta) { fClusterEta = clusterEta; }
    void SetSize      (unsigned size)    { fSize       = size;       }
    void SetBarcode   (uint64_t bc)      { fBarcode    = bc;         }

    void SetSamplingE  (const std::vector<double> &);
    void SetSamplingRaw(const std::vector<double> &);
    void SetSamplingEta(const std::vector<double> &);
    void SetSamplingPhi(const std::vector<double> &);

    void SetClusterEtEtaPhi(float et, float eta, float phi);

    // Energy deposited in (Presampler, strips, middle, back)
    float GetEs0()    const;
    float GetEs1()    const;
    float GetEs2()    const;
    float GetEs3()    const;
    float GetRawEs0() const;
    float GetRawEs1() const;
    float GetRawEs2() const;
    float GetRawEs3() const;

    std::vector<float>& GetSamplesE  () { return fSamplesE;   }
    std::vector<float>& GetSamplesRaw() { return fSamplesRaw; }
    std::vector<float>& GetSamplesEta() { return fSamplesEta; }
    std::vector<float>& GetSamplesPhi() { return fSamplesPhi; }

    const std::vector<float>& GetSamplesE  () const { return fSamplesE;   }
    const std::vector<float>& GetSamplesRaw() const { return fSamplesRaw; }
    const std::vector<float>& GetSamplesEta() const { return fSamplesEta; }
    const std::vector<float>& GetSamplesPhi() const { return fSamplesPhi; }

    unsigned GetNCell   () const { return 0;         }
    unsigned GetNCellBad() const { return 0;         }
    unsigned GetSizeEta () const { return 0;         }
    unsigned GetSizePhi () const { return 0;         }
    unsigned GetSize    () const { return fSize;     }
    uint64_t GetBarcode () const { return fBarcode;  }

    float GetClusterE  ()  const { return fClusterE;    }
    float GetClusterEt ()  const { return fClusterE/std::cosh(fClusterEta); }
    float GetClusterEta()  const { return fClusterEta;  }
    float GetClusterPhi()  const { return fClusterPhi;  }

    FourMom            Get4Mom   () const;
    FourMom            GetFourMom() const { return Get4Mom(); }

    void Clear();

    void Print(std::ostream &os = std::cout) const;

  private:    
    
    float               fClusterE;      // Cluster energy
    float               fClusterEta;    // Cluster eta
    float               fClusterPhi;    // Cluster phi
    uint16_t            fSize;          // Cluster size
        
    std::vector<float>  fSamplesE;      // Energy deposited in (presampler, strips, middle, back)
    std::vector<float>  fSamplesRaw;    // Energy deposited in (presampler, strips, middle, back)
    std::vector<float>  fSamplesEta;    // Energy deposited in (presampler, strips, middle, back)
    std::vector<float>  fSamplesPhi;    // Energy deposited in (presampler, strips, middle, back)    

    uint64_t            fBarcode;       // transient value - not saved in ntuples
  };

  //
  // Inlined functions
  //
  inline float RecCluster::GetEs0() const { 
    if(fSamplesE.size() > 3) return fSamplesE[0];
    return 0.0;
  }
  inline float RecCluster::GetEs1() const { 
    if(fSamplesE.size() > 3) return fSamplesE[1];
    return 0.0;
  }
  inline float RecCluster::GetEs2() const { 
    if(fSamplesE.size() > 3) return fSamplesE[2];
    return 0.0;
  }
  inline float RecCluster::GetEs3() const { 
    if(fSamplesE.size() > 3) return fSamplesE[3];
    return 0.0;
  }

  inline float RecCluster::GetRawEs0() const { 
    if(fSamplesRaw.size() > 3) return fSamplesRaw[0];
    return 0.0;
  }
  inline float RecCluster::GetRawEs1() const { 
    if(fSamplesRaw.size() > 3) return fSamplesRaw[1];
    return 0.0;
  }
  inline float RecCluster::GetRawEs2() const { 
    if(fSamplesRaw.size() > 3) return fSamplesRaw[2];
    return 0.0;
  }
  inline float RecCluster::GetRawEs3() const { 
    if(fSamplesRaw.size() > 3) return fSamplesRaw[3];
    return 0.0;
  }
}

#endif
