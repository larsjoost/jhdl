
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "systemc.h"

namespace vhdl {

  namespace STD {
    namespace ENV {

      void FINISH(int x) {
        std::cout << "Finish" << std::endl;
        sc_exit(x);
      }
      
    }
  }
  
  template<class T>
  class Range {
  public:
    T left;
    T right;
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
    
    std::string toString() {
      return std::to_string(value);
    }

    T getValue() {
      return value;
    }
    
    static ::std::string IMAGE(Range<T>& r) {
      return ::std::to_string(r.value);
    }

    unsigned int LENGTH() {
      return 32;
    }
    
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    iterator begin_;
    iterator end_;

  };

  template <class T, typename Units>
  class Physical {
  protected:
  public:
    T value;
    Units units;

    Physical() {};
    
    Physical(T value, Units units) :
      value(value), units(units) { }

    static ::std::string IMAGE(Physical<T, Units>& r) {
      return std::to_string(r.value) + " " + toString(r.units);
    }

  };

  template <typename T>
  class Enumeration {
  public:
    T value;

    T getValue() {
      return value;
    }
    
    void operator=(T v) {
      value = v;
    }

    static ::std::string IMAGE(Enumeration<T>& r) {
      return std::to_string(r.value);
    }
    
  };
  
  namespace STANDARD {
  
    class INTEGER : public Range<int> {
    public:
      explicit INTEGER(int left=INT_MIN, int right=INT_MAX) : Range<int>(left, right) {};
      explicit INTEGER(Range<int> r) : Range<int>(r.left, r.right) {
        value = r.value;
      };
      using Range<int>::operator=;
      using Range<int>::IMAGE;
      static ::std::string IMAGE(sc_signal<INTEGER>& r) {
        return ::std::to_string(r.signal.value);
      }

    };
    
    static INTEGER operator+(INTEGER left, const int right) {
      left.value += right;
      return left;
    }

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

    class BIT : public Enumeration<char> {
    public:
      using Enumeration<char>::operator=;
      using Enumeration<char>::IMAGE;
      
      static ::std::string IMAGE(sc_signal<BIT>& r) {
        return "'" + std::string(1, r.signal.value) + "'";
      }

      unsigned int LENGTH() {
        return 1;
      }
    };

    enum BOOLEAN_enum {TRUE, FALSE};
    
    class BOOLEAN : public Enumeration<enum BOOLEAN_enum> {
    public:
      using Enumeration<enum BOOLEAN_enum>::operator=;
      using Enumeration<enum BOOLEAN_enum>::IMAGE;
      
      static ::std::string IMAGE(sc_signal<BOOLEAN>& r) {
        return (r.signal.value == TRUE) ? "true" : "false";
      }

      BOOLEAN operator!() {
        BOOLEAN b;
        b.value = (value == TRUE ? FALSE : TRUE);
        return b;
      }
      
      operator bool() const {
        return value == TRUE;
      }
      unsigned int LENGTH() {
        return 1;
      }
    };

    enum SEVERITY_LEVEL {NOTE, WARNING, ERROR, FAILURE};

    static const std::string SEVERITY_LEVEL_STRINGS[] =
      {"NOTE", "WARNING", "ERROR", "FAILURE"};
    
    ::std::string toString(SEVERITY_LEVEL severity) {
      return SEVERITY_LEVEL_STRINGS[severity];
    }
    
    enum TIME_UNITS {FS, PS, NS, US, MS, SEC, MIN, HR};

    /*
    class TIME : public Physical<int, TIME_UNITS> {
    public:
      explicit TIME(int value=0, TIME_UNITS units=NS) : Physical<int, TIME_UNITS>(value, units) {};
    };
    */

    typedef Physical<int, TIME_UNITS> TIME;

    static TIME NOW = TIME(0, NS);

    static TIME operator+(TIME left, const TIME& right) {
      left.value += right.value;
      return left;
    }
    
    static const std::string TIME_UNITS_STRINGS[] =
      {"FS", "PS", "NS", "US", "MS", "SEC", "MIN", "HR"};
    
    ::std::string toString(TIME_UNITS units) {
      return TIME_UNITS_STRINGS[units];
    }

    
    
  }

  /*
    VHDL functions
   */
  
  void report(::std::string message, STANDARD::SEVERITY_LEVEL severity) {
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
  void wait_for(STANDARD::TIME& time) {
    auto t = STANDARD::NOW + time;
    sc_wait(time.value);
    report("Waiting until " + STANDARD::TIME::IMAGE(t), STANDARD::NOTE);
  }
  */
  /*
    Expression operator functions
   */
  
  static ::std::string concat(::std::string left, ::std::string right) {
    return left + right;
  }

  static bool equal(STANDARD::INTEGER left, int right) {
    return left.value == right;
  }

  template<typename T1, typename T2>
  static auto physical(T1 value, T2 units) -> Physical<T1, T2> {
    return Physical<T1, T2>(value, units);
  }

}

#endif

