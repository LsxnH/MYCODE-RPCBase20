// -*- c++ -*-
#ifndef ANP_RUNMODULE_H
#define ANP_RUNMODULE_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : RunModule
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 *  Top level event loop manager.
 * 
 *  Module for reads ntuple files and creates AlgEvent instances.
 *  Created algorithms go through these steps:
 *
 *  - Config() - create and configure C++ algorithms
 *  - Init()   - initialize algorithms and read configuration (Registry from python or XML)
 *  - Exec()   - event loop
 *  - Done()   - finalize algorithms
 *
 *  This function configures and executes job using input XML file:
 *
 *  - Execute() - configure input files execute above functions 
 *                using registry read from input path to XML file
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <set>
#include <vector>

// ROOT
#include "TStopwatch.h"

// Local
#include "PhysicsAnpBase/AlgEvent.h"
#include "PhysicsAnpBase/Registry.h"

class TFile;
class TTree;

namespace Anp
{
  void* DoRunModuleWork(void *run_);

  class Thread;

  class RunModule {
  public:
    
    RunModule();
    virtual ~RunModule();
    
    void Execute(const std::string &config_path);

    void Config(const Anp::Registry &reg);
    
    bool Init();
    void Exec();
    void Done();
    
    AlgEvent* GetAlg();

    void AddInputFile(const std::string &file);
    
    void ClearInputFiles();

  private:

    void ReadFile(Registry &reg, long &icount);
    void PollFile(Registry &reg);
    
    bool StopNow(long count) { return fNEvent > 0 && count+1 > fNEvent; }

    unsigned GetPathIndex(const std::string &path) const;

    std::ostream& log() const;
    
  private:

    friend void* DoRunModuleWork(void *);

  private:

    TFile                     *fFile;         // Output ROOT file pointer
    TStopwatch                 fTimerEvent;   // Event processing timer
    TStopwatch                 fTimerTotal;   // Total processing timer
    Anp::Registry              fReg;          // Global Registry configuration

    int                        fTreeSize;     // Required number of events in each TTree    
    std::string                fTreeName;     // Name of input ROOT tree(s)
    std::string                fTreePoll;     // Main tree name for file count polls
    std::string                fTreeThread;   // Name of tree for reading in seperate thread
    std::string                fReadOption;   // File read option

    Handle<AlgEvent>           fAlg;          // Algorithm to be executed

    // Properties:
    bool                       fCloseFile;
    bool                       fDebug;        // Print debug info
    bool                       fDebugThread;  // Print thread debug info
    bool                       fPrintFiles;   // Print names of input root files
    bool                       fDoThreads;    // Run threads
    
    long                       fNEvent;       // Number of events to read
    long                       fNPrint;       // Number of events to print    
    unsigned                   fNQueue;       // Size of thread data list
    unsigned                   fCompression;  // TFile compression factor

    // Variables:
    std::map<unsigned, long>   fCountMap;     // Map for counting number of processed events
    std::vector<std::string>   fInputFiles;   // Input files
    std::vector<std::string>   fInputPaths;   // Input paths
    
    Thread                    *fThread;       // Thread object - needs more work
    
    long                       fICount;       // Number of events to read
  };
}

#endif
