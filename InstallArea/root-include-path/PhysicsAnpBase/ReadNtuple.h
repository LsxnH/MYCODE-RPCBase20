// -*- c++ -*-
#ifndef ANP_READNTUPLE_H
#define ANP_READNTUPLE_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : ReadNtuple
 * @Author : Rustem Ospanov
 *
 * @Brief  : Top level algorithm to read ntuples and process event loop
 * 
 *  Module for reading flat ntuple files and creates AlgEvent instances.
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

// Data
#include "PhysicsAnpData/RecoEvent.h"
#include "PhysicsAnpData/InputInfo.h"

// Base
#include "PhysicsAnpBase/AlgEvent.h"
#include "PhysicsAnpBase/NtupleSvc.h"
#include "PhysicsAnpBase/Registry.h"
#include "PhysicsAnpBase/ReadUtils.h"

class TFile;
class TTree;

namespace Anp
{
  class ReadNtuple: public AlgEvent
  {
  public:
    
    ReadNtuple();
    virtual ~ReadNtuple();
    
    void Execute(const std::string &config_path);

    void ExecuteRegistry(const Registry &reg);

    bool OpenFile(const std::string &fpath, const std::string &treename);

    bool ReadEntry(int entry);

    void CloseFile();

    //
    // Three interface functions
    //
    bool Init();

    void Config(const Anp::Registry &reg);
    
    void Done();

  private:

    //
    // Private because this function is executed by public ReadEntry()
    //
    void Exec();

  private:
    
    bool InitTree(TTree *tree);

    long CountNEvent(const std::vector<std::string> &fpaths);

    unsigned FillReco(RecoEvent &event);

    bool StopNow(long count) const { return fNEvent > 0 && count+1 > fNEvent; }

    bool SkipEvent(long count) const;

  private:

    enum KeyBranch 
    {
      kNone,
      kCluster,
      kJet,
      kElec,
      kMuon,
      kTau,
      kTrack,
      kTrackJet,
      kVtx,
      kHit,
      kTruthPart,
      kTruthVtx,
      kTruthJet
    };

  private:

    typedef std::vector<VarData>  VarVec;
    typedef std::vector<VecData>  VecVec;
    typedef std::set<unsigned>    VarSet;

    struct Group
    {
      Group(const std::string &_list_prefix, const std::string &group_key);

      unsigned GetGroupLength(unsigned index) const;

      const std::string group_key;
      const std::string group_prefix;
      const unsigned    group_var;

      VecVec            group_vecs;
    };

    typedef std::vector<Group> GroupVec;

    struct List
    {
      List(const std::string &p, KeyBranch k):prefix(p), bkey(k) {}

      void ActivateList(TTree *tree);

      void ClearList(bool debug);

      unsigned GetListVectorSize() const;

      bool MatchBranchKey(KeyBranch key) const { return bkey == key; }
      
      void Print() const;

      const std::string prefix;
      const KeyBranch   bkey;

      VecVec            var_vecs;
      GroupVec          var_groups;
    };

    typedef std::vector<List>        ListVec;
    typedef std::vector<VarNick>     NickVec;
    typedef std::vector<std::string> StrVec;

    typedef std::set<std::pair<std::string, std::string> > StrPairSet;

  private:

    template<class T> unsigned FillObjVec(const List &l, std::vector<Ptr<T> > &vec);

    void ReadNick(const Registry &reg);

    unsigned FindVar(const std::string &val, const VarSet &vetos) const;

    std::pair<bool, std::string> MatchBranchNameToPrefix(const std::string &branch, const std::string &prefix) const;

    bool MatchListToBranch(List &l, TBranch *b, Read::Type branch_type) const;

    KeyBranch GetKeyBranch(const std::string &prefix) const;

    void PrintBranches(TTree *tree) const;    

    void PrintDebugVars() const;

  private:    

    TFile                     *fFile;               // Output ROOT file pointer

    TFile                     *fInputFile;          // Input TFile
    TTree                     *fInputTree;          // Input TTree

    TStopwatch                 fTimerEvent;         // Event processing timer
    TStopwatch                 fTimerExec;          // Exec  processing timer for algrorithms
    TStopwatch                 fTimerTotal;         // Total processing timer

    Registry                   fReg;                // Global Registry configuration

    std::string                fOutputFile;         // Name of output ROOT file
    std::string                fTreeName;           // Name of input ROOT tree
    std::string                fNtupleInstance;     // Name of NtupleSvc::Instance
    std::string                fCpuProfile;         // Name of profiler file

    std::string                fPrefixHit;          // Prefix for vector of detector hits
    std::string                fPrefixCluster;      // Prefix for vector of calorimeter clusters
    std::string                fPrefixJet;          // Prefix for vector of calorimeter jets
    std::string                fPrefixElec;         // Prefix for vector of reconstructed electrons
    std::string                fPrefixMuon;         // Prefix for vector of reconstructed muons
    std::string                fPrefixTau;          // Prefix for vector of reconstructed taus
    std::string                fPrefixTrack;        // Prefix for vector of reconstructed ID tracks
    std::string                fPrefixTrackJet;     // Prefix for vector of reconstructed track jets
    std::string                fPrefixVtx;          // Prefix for vector of reconstructed vertexes
    std::string                fPrefixTruthPart;    // Prefix for vector of truth particles
    std::string                fPrefixTruthVtx;     // Prefix for vector of truth vtxs
    std::string                fPrefixTruthJet;     // Prefix for vector of truth jets

    Branch<RecoEvent>          fReco;               // Output RecoEvent
    Branch<InputInfo>          fInfo;               // Input info for file, tree and entry
 
    Handle<AlgEvent>           fAlg;                // Top level event algorithm
    
    VarSet                     fVetoVars;
    VarSet                     fVetoVecs;

    VarVec                     fVars;
    NickVec                    fNicks;

    ListVec                    fLists;

    // Properties:
    bool                       fDebug;              // Print debug info for algorithms
    bool                       fDebugVars;          // Print debug info for variables
    bool                       fCloseFile;
    bool                       fPrintFiles;         // Print names of input root files
    bool                       fFillTrueParts;      // Enable/disable reading/filling of truth particles
    bool                       fPrintObjectFactory; // Print ObjectFactory summary at shutdown

    long                       fNEvent;             // Maximum number of events to read
    long                       fNEventPerFile;      // Maximum number of events to read per file (for tests)
    long                       fNPrint;             // Number of events to print    
    unsigned                   fCompression;        // TFile compression factor
    double                     fEventFracMin;
    double                     fEventFracMax;

    // Variables:
    std::vector<std::string>   fInputFiles;         // Input files    
    long                       fICount;             // Number of events to read

    StrPairSet                 fDuplicateBranches;  // Store here duplicate branches
  };

  //------------------------------------------------------------------------------------
  // Inlined functions
  //
  inline bool ReadNtuple::SkipEvent(long count) const 
  {
    if(!(fEventFracMin < fEventFracMax)) {
      return false;
    }
    
    if(fNEvent < 1) {
      return false;
    }
    
    const double ifrac = count/double(fNEvent);
    
    return !(fEventFracMin <= ifrac && ifrac < fEventFracMax);
  }
}

#endif
