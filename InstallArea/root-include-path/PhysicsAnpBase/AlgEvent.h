// -*- c++ -*-
#ifndef ANP_ALGEVENT_H
#define ANP_ALGEVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpBase
 * @Class  : AlgEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Base class for event algorithms
 *
 * Order of execution:
 *
 * 1) Config() - called once with Registry configuration from macro
 *
 * 2) Save()   - called once after config if ouput ROOT is created
 *
 * 3) Init()   - called after Config() and Save() but before first Exec()
 *
 * 4) Exec()   - called for every TTree entry read from input files
 *
 * 5) Done()   - called once after all TTrees are read or maximum number of events is processed
 *
 **********************************************************************************/

// C/C++
#include <typeinfo>

// ROOT
#include "TStopwatch.h"

// Local
#include "PhysicsAnpBase/Handle.h"
#include "PhysicsAnpBase/Factory.h"
#include "PhysicsAnpBase/Registry.h"
#include "PhysicsAnpBase/UtilBase.h"

class TDirectory;

namespace Anp
{

  enum LogType {
    Init = 0,
    Config,
    Exec,
    Done,
    NONE
  };

  class Callback
  {
  public:   
    Callback() {}
    virtual ~Callback() {}
    
    virtual void Signal(const Anp::Registry &, const std::string &) {}

    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetType() const = 0;
  };

  class AlgEvent: public virtual Base, public virtual Callback
  {
  public:   
      
    AlgEvent();
    virtual ~AlgEvent() {}

    virtual void Config(const Anp::Registry &) {}

    virtual void Save(TDirectory *) {}

    virtual bool Init() = 0;
    
    virtual void Exec() = 0;
    
    virtual void Done() {}
    
    virtual std::ostream& log(LogType type=NONE) const;   

    virtual const std::string& GetName() const { return fName; }
    virtual const std::string& GetType() const { return fType; }
    
    virtual uint32_t GetNameId() const { return fNameId; }
    virtual uint32_t GetTypeId() const { return fTypeId; }

    virtual void SetName(const std::string &val);
    virtual void SetType(const std::string &val);

    TStopwatch& GetTimerExec() { return fTimerExec; }
    
  protected:

    typedef std::vector<Handle<AlgEvent> > AlgVec;

    AlgVec      fSubAlgs;

  protected:

    Handle<AlgEvent> GetAlg(const std::string &name) const;

    template<class T> Handle<T> BookAlg(const std::string &key,
					const Registry &reg,
					const std::string &name = "");

    template<class T> Handle<T> BookAlg(const std::string &alg_type,
					const std::string &alg_name,
					const Registry &reg);
					

  private:

    std::string fName;
    std::string fType;

    uint32_t    fNameId;
    uint32_t    fTypeId;

    TStopwatch  fTimerExec;    
  };
  
  //------------------------------------------------------------------------------
  // Special method to register callbacks functions
  //
  template<class T> inline bool RegisterCallback(const Registry &reg,
						 const std::string& key, 
						 Handle<T> &val, 
						 Callback *caller)
    {
      //
      // Attempt to retrieve and register algorithm for callback
      //
      
      std::string alg_name;
      if(key.find("::") != std::string::npos) {
	//
	// Split key using 
	//
	std::vector<std::string> keys;
	Anp::StringTok(keys, key, "::");

	if(keys.size() == 2) {
	  reg.Get(keys.at(0), keys.at(1), alg_name);
	}
	else {
	  return false;
	}
      }
      else {
	reg.Get(key, alg_name);
      }
      
      if(alg_name.empty()) {
	return false;
      }
      
      val = Factory<T>::Instance().Get(alg_name);
      
      if(val.valid()) {
	val->RegisterCallback(caller);
	return true;
      }
      
      return false;
    }

  inline Handle<AlgEvent> AlgEvent::GetAlg(const std::string &name) const
    {
      for(unsigned i = 0; i < fSubAlgs.size(); ++i) {
	const Handle<AlgEvent> &alg = fSubAlgs.at(i);
	if(alg->GetName() == name) {
	  return alg;
	}
      }

      return Handle<AlgEvent>();
    }

  //------------------------------------------------------------------------------
  // Book sub-algorithm - called by derived algorithm class
  //
  template<class T> inline Anp::Handle<T> AlgEvent::BookAlg(const std::string &key, 
							    const Registry &reg,
							    const std::string &name)
    {
      //
      // Create new algorithm
      //
      Registry alg_reg;           
      if(!reg.Get(key, alg_reg)) {
	if(!reg.Get(GetType()+"::"+key, alg_reg)) {
	  log() << "BookAlg<T> - error: missing Registry for \"" << key << "\"" << std::endl;
	  return Handle<T>();
	}
      }

      std::string alg_name;
      std::string alg_type;

     if(!name.empty()) {
	alg_name = name;
     }
     else {
       if(!alg_reg.Get("AlgName", alg_name)) {
	 log() << "BookAlg<T> - missing AlgName" << std::endl;
	 return Handle<T>();
       }
     }
     if(!alg_reg.Get("AlgType", alg_type)) {
	log() << "BookAlg<T> - missing AlgType" << std::endl;
	return Handle<T>();
      }

      if(GetAlg(alg_name).valid()) {
	log() << "BookAlg<T> - ignore duplicate algorithm name: " << alg_name << std::endl;
	return Handle<T>();
      }
      
      Handle<T> ptr(new T());
      ptr -> SetType(alg_type);
      ptr -> SetName(alg_name);
       
      ptr -> Config(alg_reg);
      
      Handle<AlgEvent> alg(ptr.get());
      fSubAlgs.push_back(alg);
      
      return ptr;
    }

  //------------------------------------------------------------------------------
  // Book sub-algorithm - called by derived algorithm class
  //
  template<class T> inline Anp::Handle<T> AlgEvent::BookAlg(const std::string &alg_type,
							    const std::string &alg_name,
							    const Registry &reg)
    {
      //
      // Create new algorithm
      //
      if(GetAlg(alg_name).valid()) {
	log() << "BookAlg<T> - ignore duplicate algorithm name: " << alg_name << std::endl;
	return Handle<T>();
      }
            
      Handle<AlgEvent> alg = Factory<AlgEvent>::Instance().Create(alg_type);
      
      if(!alg.valid()) {
      	log() << "BookAlg<T> - failed to create: " << alg_name << "/" << alg_type << std::endl;
	exit(1);
      }

      T *talg = dynamic_cast<T *>(alg.get());

      if(!talg) {
      	log() << "BookAlg<T> - failed to cast " << alg.get() << " (" << typeid(alg.get()).name() << ") to " 
	      << alg_name << "/" << alg_type << std::endl;
	exit(1);
      }

      Handle<T> ptr(talg);
      ptr -> SetType(alg_type);
      ptr -> SetName(alg_name);       
      ptr -> Config(reg);
      
      fSubAlgs.push_back(alg);
      
      return ptr;
    }

  //------------------------------------------------------------------------------
  // Book sub-algorithm - called by derived algorithm class
  //
  template<> inline Anp::Handle<AlgEvent> AlgEvent::BookAlg<AlgEvent>(const std::string &key, 
								      const Registry &reg,
								      const std::string &name)
    {
      //
      // Create new algorithm
      //
      Registry alg_reg;      
      if(!reg.Get(key, alg_reg)) {
	if(!reg.Get(GetType()+"::"+key, alg_reg)) {
	  log() << "BookAlg<AlgEvent> - error: missing Registry for: " << key << std::endl;
	  return Handle<AlgEvent>();
	}
      }

      std::string alg_name;
      std::string alg_type;

      if(!name.empty()) {
	alg_name = name;
      }
      else {
	if(!alg_reg.Get("AlgName", alg_name)) {
	  log() << "BookAlg<AlgEvent> - missing AlgName" << std::endl;
	  return Handle<AlgEvent>();
	}
      }
      if(!alg_reg.Get("AlgType", alg_type)) {
	log() << "BookAlg<AlgEvent> - missing AlgType" << std::endl;
	return Handle<AlgEvent>();
      }

      if(GetAlg(alg_name).valid()) {
	log() << "BookAlg<AlgEvent> - ignore duplicate algorithm name: " << alg_name << std::endl;
	return Handle<AlgEvent>();
      }

      Handle<AlgEvent> ptr = Factory<AlgEvent>::Instance().Create(alg_type);
      
      if(ptr.valid()) {
	ptr -> SetType(alg_type);
	ptr -> SetName(alg_name);
	ptr -> Config(alg_reg);

	fSubAlgs.push_back(ptr);
      }
      else {
	log() << "BookAlg<AlgEvent> - failed to create algorithm: " << key << std::endl;
      }
      
      return ptr;
    }
}

#endif
