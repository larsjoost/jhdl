
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
  public:
    T value;

    class iterator {
      friend class Range;
      Range<T>* parent;
      T index;
    public:
      Range<T> operator *() const { parent->value = index; return *parent; }
      const iterator &operator ++() { ++index; return *this; }
      Range<T> operator ++(T) { iterator copy(*this); ++index; return copy; }
      
      bool operator ==(const iterator &other) const { return index == other.index; }
      bool operator !=(const iterator &other) const { return index != other.index; }

    protected:
      iterator(Range<T>* parent, T start) : parent(parent), index(start) { }
    };
    
  public:
      
    Range(T left, T right) :
      value(left), left(left), right(right),
      begin_(this, left), end_(this, right + 1) { }
    
    void operator=(T v) {
      value = v;
    }
    
    static ::std::string IMAGE(Range<T> r) {
      return ::std::to_string(r.value);
    }
    
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
  private:
    iterator begin_;
    iterator end_;

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
    std::ostream* o;
    if (severity == STANDARD::ERROR || severity == STANDARD::FAILURE) {
      o = &std::cerr;
    } else {
      o = &std::cout;
    } 
    *o << STANDARD::toString(severity) << ": " << message << ::std::endl;
    if (severity == STANDARD::FAILURE) {
      STD::ENV::FINISH(1);
    } 
  }

  /*
    Expression operator functions
   */
  
  static ::std::string concat(::std::string left, ::std::string right) {
    return left + right;
  }
  
  static STANDARD::INTEGER add(auto left, auto right) {
    auto sum = left + right;
    STANDARD::INTEGER l;
    l = sum;
    return l;
  }
  
  static bool equal(STANDARD::INTEGER left, int right) {
    return left.value == right;
  }
}

#endif
