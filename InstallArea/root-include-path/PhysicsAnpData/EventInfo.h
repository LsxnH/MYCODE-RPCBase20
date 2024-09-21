// This is -*- c++ -*-
#ifndef ANP_EVENTINFO_H
#define ANP_EVENTINFO_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : CandEvent
 * @Author : Josh Kunkle, Rustem Ospanov
 *
 * @Brief  : 
 * 
 *  EventInfo stores basic event information
 *  
 * 
 **********************************************************************************/

// C/C++
#include <string>

namespace Anp
{
  namespace EventFlags
  {
    enum Flags {
      NONE                = 0,
      IsCalibration       = 0x1,
      IsData              = 0x2,
      IsSimulation        = 0x4
    };

    Flags       Convert2Flag  (const std::string &flag);
    std::string Convert2String(const Flags flag); 
  }

  class EventInfo
  {
  public:

    EventInfo(); 
    virtual ~EventInfo() {}
    
    void SetWeight       (double     weight)      { fWeight        = weight;      }
    void SetMCEventNumber(unsigned   eventNumber) { fMCEventNumber = eventNumber; }
    void SetMCChannel    (unsigned   channel)     { fMCChannel     = channel;     }
    void SetRun          (unsigned   run)         { fRun           = run;         }
    void SetLB           (unsigned   lb)          { fLB            = lb;          }
    void SetEvent        (uint64_t   event)       { fEvent         = event;       }
    void SetBCID         (unsigned   bcid)        { fBCID          = bcid;        }

    void AddBit  (EventFlags::Flags flag)       {        fBits |= flag; }
    bool CheckBit(EventFlags::Flags flag) const { return fBits  & flag; }

    bool IsSimulation () const { return CheckBit(EventFlags::IsSimulation);       }
    bool IsData       () const { return CheckBit(EventFlags::IsData);             }
    bool IsMC         () const { return IsSimulation();                           }
    bool IsCalibration() const { return CheckBit(EventFlags::IsCalibration);      }

    double   GetWeight       () const { return fWeight;        }
    uint32_t GetMCChannel    () const { return fMCChannel;     }
    uint32_t GetMCEventNumber() const { return fMCEventNumber; }
    uint32_t GetRun          () const { return fRun;           }
    uint32_t GetLB           () const { return fLB;            }
    uint64_t GetEvent        () const { return fEvent;         }
    uint32_t GetBCID         () const { return fBCID;          }

  private:

    double            fWeight;
    uint32_t          fMCEventNumber;
    uint32_t          fMCChannel;

    uint32_t          fRun;
    uint32_t          fLB;
    uint64_t          fEvent;
    uint32_t          fBCID;

    uint32_t          fBits;   
  };
}

#endif
