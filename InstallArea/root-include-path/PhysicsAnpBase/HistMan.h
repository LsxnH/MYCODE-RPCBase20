// -*- c++ -*-
#ifndef ANP_HISTMAN_H
#define ANP_HISTMAN_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : HistMan
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 *  HistMan manages creation of ROOT histograms. 
 *  Histogram definitions are loaded from XML files.
 *  
 * 
 **********************************************************************************/

// C++
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Data
#include "PhysicsAnpData/Ptr.h"

// Local
#include "PhysicsAnpBase/VarExpr.h"
#include "PhysicsAnpBase/VarCond.h"

class TDirectory;
class TH1;
class TH2;
class TH3;

namespace Anp
{   
  class Registry;

  //================================================================================================================
  struct HistVars2d
  {
    HistVars2d();

    TH2      *h;
    unsigned  key_varx;
    unsigned  key_vary;
  };
  
  //================================================================================================================
  class AxisInfo
  {
  public:
    
    AxisInfo();
    ~AxisInfo();
    
    const std::string& GetTitle  () const { return fTitle;   }
    const std::string& GetVarName() const { return fVarName; }
    
    double   GetMin   () const { return fMin;    }
    double   GetMax   () const { return fMax;    }
    int      GetNbins () const { return fNbins;  }
    unsigned GetVarKey() const { return fVarKey; }
    
    bool Valid() const { return fValid; }
    
    const std::vector<double>& GetBins () const { return fBins; }
    const double              *GetArray() const;

    void Print(std::ostream& o = std::cout) const;
    
  private:
    
    friend class HistMan;
    
    int      fNbins;
    double   fMin;
    double   fMax;
    unsigned fVarKey;

    bool   fValid;
    
    std::vector<double> fBins;
    double             *fArray;
    
    std::string fTitle;
    std::string fVarName;
  };

  //================================================================================================================
  class HistInfo
  {
  public:
    
    HistInfo() {}
    ~HistInfo() {}
    
    const std::string& GetName () const { return fName;  }
    const std::string& GetTitle() const { return fTitle; }
    const std::string& GetKey  () const { return fKey;   }
    
    const AxisInfo& GetXaxis() const { return fXaxis; }
    const AxisInfo& GetYaxis() const { return fYaxis; }
    const AxisInfo& GetZaxis() const { return fZaxis; }   
    
    bool operator <(const HistInfo &rhs) const { return fKey  < rhs.fKey; }
    bool operator==(const HistInfo &rhs) const { return fKey == rhs.fKey; }
    
    void Print(std::ostream& o = std::cout) const;
    
  private:

    friend class HistMan;
    
    std::string fKey;   
    std::string fName;
    std::string fTitle;
    
    AxisInfo fXaxis;
    AxisInfo fYaxis;
    AxisInfo fZaxis;
  };

  //================================================================================================================
  class ExprInfo
  {
  public:
    
    ExprInfo() {}
    ~ExprInfo() {}
    
    const std::string& GetName   () const { return fName;    }
    const std::string& GetHist   () const { return fHist;    }
    const std::string& GetFormula() const { return fFormula; }
    
    bool operator <(const ExprInfo &rhs) const { return fName  < rhs.fName; }
    bool operator==(const ExprInfo &rhs) const { return fName == rhs.fName; }
    
    friend class HistMan;
    
  protected:
    
    std::string fName;
    std::string fFormula;
    std::string fHist;
  };

  //================================================================================================================
  class CondInfo
  {
  public:
    
    CondInfo() {}
    ~CondInfo() {}
    
    const std::string& GetName() const { return fName; }
    const std::string& GetExpr() const { return fExpr; }
    const std::string& GetCond() const { return fCond; }
    const std::string& GetHist() const { return fHist; }
    
    bool operator <(const CondInfo &rhs) const { return fName  < rhs.fName; }
    bool operator==(const CondInfo &rhs) const { return fName == rhs.fName; }
    
    friend class HistMan;
    
  protected:
    
    std::string fName;
    std::string fExpr;
    std::string fCond;
    std::string fHist;
  };
  
  //================================================================================================================
  // Histogram manager class
  //
  class HistMan
  {    
  public:
    
    static HistMan& Instance();
    
    void Config(const Registry &reg);
    
    bool ReadFile(const std::string file);
    
    bool KeyExists(const std::string &key, const std::string &dir) const;
    
    unsigned NMiss() const { return fNMiss; }
    
    TH1* CreateTH1(const std::string &name, const std::string &dir);
    TH2* CreateTH2(const std::string &name, const std::string &dir);
    TH3* CreateTH3(const std::string &name, const std::string &dir);
    
    TH2* GetTH2(const std::string &xkey, const std::string &ykey,                          const std::string &dir);
    TH3* GetTH3(const std::string &xkey, const std::string &ykey, const std::string &zkey, const std::string &dir);

    std::vector<TH1 *> CreateTH1VectorForOneKey(const std::string &key, const std::string &dir);

    std::vector<HistVars2d> CreateHistVars2d(const std::string &dir);

    Ptr<VarExpr> CreateExpr(const std::string &key, const std::string &dir);
    Ptr<VarCond> CreateCond(const std::string &key, const std::string &dir);

    std::vector<Ptr<VarExpr> > CreateExprs(const std::string &dir);
    std::vector<Ptr<VarCond> > CreateConds(const std::string &dir);
    
    void Print(std::ostream& o = std::cout) const;
    
    const std::vector<std::string> GetHistKeys2d(const std::string &dir);
    
  private:     
    
    HistMan();
    ~HistMan();
    
    HistMan(const HistMan &);
    const HistMan& operator=(const HistMan &);
    
  private:

    typedef std::map<std::string, HistInfo> InfoMap;
    typedef std::map<std::string, ExprInfo> ExprMap;
    typedef std::map<std::string, CondInfo> CondMap;

    struct DirData
    {
      InfoMap infos;
      ExprMap exprs;
      CondMap conds;
    };

    typedef std::map<std::string, DirData>  DirMap;

    typedef InfoMap::const_iterator InfoIter;
    typedef DirMap::const_iterator DirIter;

  private:
    
    TH1* CreateTH1(const HistInfo &info, const std::string &opt = "double");
    TH2* CreateTH2(const HistInfo &info, const std::string &opt = "float"); 
    TH3* CreateTH3(const HistInfo &info, const std::string &opt = "float"); 
    
    bool ReadAxis(const std::string &axis_key, const Registry &top_reg, AxisInfo &info) const;
    
  private:
    
    bool       fDebug;
    bool       fPrint;
    
    unsigned   fNMiss;
    
    DirMap     fDirMap;
  }; 
  
  TH1* MakeTH1(const std::string &dir, TDirectory *dir_, const std::string &key);
  TH2* MakeTH2(const std::string &dir, TDirectory *dir_, const std::string &key);
  TH3* MakeTH3(const std::string &dir, TDirectory *dir_, const std::string &key);
  
  std::ostream& operator<<(std::ostream& o, const HistInfo &self);
  std::ostream& operator<<(std::ostream& o, const AxisInfo &self);
}

#endif
