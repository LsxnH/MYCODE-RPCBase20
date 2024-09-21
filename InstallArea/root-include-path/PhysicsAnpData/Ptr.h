// -*- c++ -*-
#ifndef ANP_ANPPTR_H
#define ANP_ANPPTR_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : Ptr
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Ptr template - smart non-intrusive reference counting pointer 
 *   - when reference count drops to 0, return object to ObjectFactory pool
 *
 **********************************************************************************/

// C++
#include <iostream>
#include <typeinfo>
#include <vector>

namespace Anp
{
  template <class T> class ObjectFactory;

  //
  // Reference counting smart pointer - objects are recycled using factory
  //
  template <typename T> class Ptr
  {
  private:
    //
    // ctor from a raw pointer - only accesible through factory
    //
    Ptr(T* ptr_, int *count_);

    friend class ObjectFactory<T>;

    template <class Out, class In> friend Ptr<Out> DynamicCastPtr(const Ptr<In> &);

  public:
    
    //
    // default ctor
    //
    Ptr() :ptr(0), count(0) {}
      
    //
    // copy ctor
    //        
    Ptr(const Ptr& rhs);
    
    //
    // dtor
    //
    ~Ptr();

    //
    // assignment operator
    //
    const Ptr& operator=(const Ptr& rhs);
    
    //
    // operator-> allows usage with pointer semantics
    // 
    T* operator->() const { return ptr; }

    //
    // provides access to pointer
    // 
    T* get() const { return ptr; }
    
    //
    // dereference operator
    //
    T& operator*() const { return *ptr; }

    //
    // provides access to reference
    // 
    const T& ref() const { return *ptr; }
          T& ref()       { return *ptr; }

    //
    // check if pointer is set to 0
    //
    bool valid() const { return ptr; }
    
    //
    // release - set pointer to 0
    //
    void release();

    //
    // deep copy - make new Ptr with copied data
    //
    Ptr<T> MakeCopy() const;

    //
    // return current reference count (-1 for invalid pointer)
    //
    int get_count() const;

  private:

    void init();

  private:

    T   *ptr;    // Pointer to data
    int *count;  // Pointer to reference count
  };

  void PrintObjectFactorySummary();
  
  //----------------------------------------------------------------------------------------------
  // Object factory base class
  //
  class ObjectFactoryBase
  {
  public:

    ObjectFactoryBase() {}
    virtual ~ObjectFactoryBase() {}

    virtual void Clear() = 0;

    virtual void PrintSummary(std::ostream &os) const = 0;
  };

  //----------------------------------------------------------------------------------------------
  // Object factory - re-use objects with pool
  //
  template<class T> class ObjectFactory: public ObjectFactoryBase
  {       
  public:
    
    static ObjectFactory& Instance();
    
    Ptr<T> CreateObject();
    
    Ptr<T> CreateObjectNew();
    
    Ptr<T> CreateObject(const T &);
    
    void SetDebug(bool flag) { fDebug = flag; }
    
    void Clear();

    void ClearDeep();
    
    void PrintSummary(std::ostream &os) const;
    
  private:
    
    void HoldObject(T *ptr, int *count);
    
  private:
    
    //
    // Use Instance() method to access/create Factory
    //
    ObjectFactory();
    virtual ~ObjectFactory() {}
    
    //
    // These two methods are private and not defined by design
    //
    ObjectFactory(const ObjectFactory &);
    const ObjectFactory& operator =(const ObjectFactory &);
    
  private:
    
    friend class Ptr<T>;
    
  private:
    
    struct PoolData
    {
      PoolData() : pool_ptr(0), pool_count(0) {}
      PoolData(T *ptr, int *count) : pool_ptr(ptr), pool_count(count) {}
      
      T   *pool_ptr;
      int *pool_count;
    };
    
    typedef std::vector<PoolData> PoolVec; 
    
    PoolVec   fPool;          // Pool of available T object pointers
    bool      fDebug;         // Print debugging info
    bool      fDoNotHold;     // This factory instance does not hold any objects
    
    unsigned  fCountCreate;   // Count create calls
    unsigned  fCountHold;     // Count hold calls
    unsigned  fCountNew;      // Count new operator calls
  };

  //----------------------------------------------------------------------------------------------
  // Implementations of template functions
  //
  inline std::vector<Anp::ObjectFactoryBase *>& GetObjectFactoryList()
  { 
    static std::vector<Anp::ObjectFactoryBase *> FactoryList;
    return FactoryList; 
  }
  
  //----------------------------------------------------------------------------------------------
  // Implementations of template functions
  //  
  template<class T> ObjectFactory<T>::ObjectFactory():
    fDebug      (false),
    fDoNotHold  (false),
    fCountCreate(0),
    fCountHold  (0),
    fCountNew   (0)
  { 
    Anp::GetObjectFactoryList().push_back(this);
  } 

  //----------------------------------------------------------------------------------------------
  template<class T> ObjectFactory<T>& ObjectFactory<T>::Instance()
  {
    static Anp::ObjectFactory<T> gInstance;
    return gInstance;
  }

  //----------------------------------------------------------------------------------------------  
  template<class T> Ptr<T> ObjectFactory<T>::CreateObject()
  { 
    ++fCountCreate;
    
    if(fDebug) {
      std::cout << "CreateObject - create new object" << std::endl;
    }
    
    if(fPool.empty()) {
      ++fCountNew;
      return Ptr<T>(new T, new int(0));
    }
    
    if(fDebug) {
      std::cout << "CreateObject - use object from pool" << std::endl;
    }
    
    Ptr<T> h(fPool.back().pool_ptr, fPool.back().pool_count);
    fPool.pop_back();
    
    static T initT;
    h.ref() = initT;
    
    return h;
  }

  //----------------------------------------------------------------------------------------------  
  template<class T> Ptr<T> ObjectFactory<T>::CreateObjectNew()
  {
    ++fCountCreate;
    
    if(fDebug) {
      std::cout << "CreateObjectNew - create new object" << std::endl;
    }
    
    ++fCountNew;
    return Ptr<T>(new T, new int(0));
  }

  //----------------------------------------------------------------------------------------------    
  template<class T> Ptr<T> ObjectFactory<T>::CreateObject(const T &obj) 
  { 
    ++fCountCreate;
    
    if(fDebug) {
      std::cout << "CreateObject - copy-create new object" << std::endl;
    }

    if(fPool.empty()) {
      ++fCountNew;
      return Ptr<T>(new T(obj), new int(0));
    }
      
    Ptr<T> h(fPool.back().pool_ptr, fPool.back().pool_count);
    fPool.pop_back();
    
    h.ref() = obj;
    
    return h;
  }

  //----------------------------------------------------------------------------------------------    
  template<class T> void ObjectFactory<T>::HoldObject(T *ptr, int *count)
  { 
    if(fDebug) {
      std::cout << "HoldObject - holding object: " << fCountHold << std::endl;
    }
    
    ++fCountHold;
    fPool.push_back(PoolData(ptr, count));
  }
  
  //----------------------------------------------------------------------------------------------  
  template<class T> void ObjectFactory<T>::Clear()
  {
    fCountCreate = 0;
    fCountHold   = 0;
    fCountNew    = 0;       

    fPool.clear();
  }

  //----------------------------------------------------------------------------------------------  
  template<class T> void ObjectFactory<T>::ClearDeep()
  {
    fCountCreate = 0;
    fCountHold   = 0;
    fCountNew    = 0;       

    for(PoolData &p: fPool) {
      if(p.pool_ptr) {
	delete p.pool_ptr;
	delete p.pool_count;
      }
    }

    fPool.clear();
  }
  
  template<class T> void ObjectFactory<T>::PrintSummary(std::ostream &os) const
  {
    os << "ObjectFactory<" << typeid(T).name() << "> - usage summary:" << std::endl
       << "   create count: " << fCountCreate << std::endl
       << "   hold   count: " << fCountHold   << std::endl
       << "   new    count: " << fCountNew    << std::endl
       << "   pool size:    " << fPool.size() << std::endl;	  
  }

  //----------------------------------------------------------------------------------------------
  //
  // Ptr template implementation
  //
  template <typename T> Ptr<T>::Ptr(T *ptr_, int *count_)
    :ptr(ptr_), count(count_)
  {
    init();
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> Ptr<T>::Ptr(const Ptr& rhs)
  {
    ptr   = rhs.ptr;
    count = rhs.count;
    init();
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> Ptr<T>::~Ptr()
  {
    if(ptr) {
      release();
    }
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> const Ptr<T>& Ptr<T>::operator=(const Ptr& rhs)
  {
    if (ptr != rhs.ptr) {
      if (ptr) {
	release();
      }
      
      ptr   = rhs.ptr;
      count = rhs.count;
      init();
    }
    
    return *this;
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> void Ptr<T>::init()
  {
    if(ptr) ++(*count);
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> void Ptr<T>::release()
  {
    if(ptr) {
      if(*count == 0 || --(*count) == 0) {
	ObjectFactory<T>::Instance().HoldObject(ptr, count);
      }
    }
    
    ptr   = 0;
    count = 0;
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> int Ptr<T>::get_count() const
  {
    if(count) {
      return *count;
    }
    
    return -1;
  }

  //----------------------------------------------------------------------------------------------    
  template <typename T> Ptr<T> Ptr<T>::MakeCopy() const
  {
    if(!valid()) {
      return Ptr<T>();
    }

    return ObjectFactory<T>::Instance().CreateObject(ref());
  }

  //------------------------------------------------------------------------------------------
  // Global functions
  //------------------------------------------------------------------------------------------
  template <class Out, class In> Ptr<Out> DynamicCastPtr(const Ptr<In> &in)
  {
    Out *optr = dynamic_cast<Out *>(in.ptr);
    
    if(!optr) {
      return Ptr<Out>();
    }
    
    return Ptr<Out>(optr, in.count);
  }
   
  //
  // Ptr<T> can be printed with "cout << handle;"
  //
  template <typename T>
  std::ostream& operator<<(std::ostream& o, const Ptr<T>& h)
  {
    if(h.valid()) o << h.ref();
    return o;
  }
}

#endif



