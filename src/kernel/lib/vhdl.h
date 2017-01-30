
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

namespace vhdl {

  namespace STD {
    namespace ENV {

      void FINISH(int x) {
        exit(x);
      }
      
    }
  }

  template<class T>
  class Range {
    T left;
    T right;
  protected:
    T value;
  public:
    Range(T left, T right) {
      value = left;
      this->left = left;
      this->right = right;
    };

    void operator=(T v) {
      value = v;
    }

    static ::std::string IMAGE(Range<T> r) {
      return ::std::to_string(r.value);
    }
    
  };

  namespace STANDARD {
  
    class INTEGER : public Range<int> {
    public:
      explicit INTEGER(int left=INT_MIN, int right=INT_MAX) : Range<int>(left, right) {};
      using Range<int>::operator=;
    };
    
    class NATURAL : public INTEGER {
    public:
      explicit NATURAL(int left=0, int right=INT_MAX) : INTEGER(left, right) {};
      using INTEGER::operator=;
    };

    class POSITIVE : public INTEGER {
    public:
      explicit POSITIVE(int left=1, int right=INT_MAX) : INTEGER(left, right) {};
      using INTEGER::operator=;
    };

    enum SEVERITY_LEVEL {NOTE, WARNING, ERROR, FAILURE};

    ::std::string toString(SEVERITY_LEVEL severity) {
      switch (severity) {
      case NOTE: return "NOTE"; break;
      case WARNING: return "WARNING"; break;
      case ERROR: return "ERROR"; break;
      case FAILURE: return "FAILURE"; break;
      }  
    }
    
  }

  /*
    VHDL functions
   */
  
  void REPORT(::std::string message, STANDARD::SEVERITY_LEVEL severity) {
    ::std::cout << STANDARD::toString(severity) << ": " << message << ::std::endl;
    if (severity == STANDARD::FAILURE) {
      STD::ENV::FINISH(1);
    }
  }

  /*
    Expression operator functions
   */
  
  ::std::string CONCAT(::std::string left, ::std::string right) {
    return left + right;
  }
  
}

#endif
