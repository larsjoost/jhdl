
#ifndef VHDL_STANDARD_H
#define VHDL_STANDARD_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_type.hpp"

namespace vhdl {

  namespace STANDARD {
  
    class INTEGER : public Range<int> {
    public:
      explicit INTEGER(int left=INT_MIN, int right=INT_MAX) : Range<int>(left, right) {};
      explicit INTEGER(Range<int> r) : Range<int>(r.left, r.right) {
        value = r.value;
      };
      INTEGER(int v) : Range<int>(INT_MIN, INT_MAX) {
        value = v;
      }
      
      using Range<int>::operator=;
      using Range<int>::operator==;
      using Range<int>::operator!=;
      using Range<int>::IMAGE;

      INTEGER operator +(int other) { value += other; return *this;}
      INTEGER operator -(int other) { value -= other; return *this;}
      INTEGER operator +(INTEGER other) { value += other.value; return *this;}
      INTEGER operator -(INTEGER other) { value -= other.value; return *this;}

      std::string toString() {
        return ::std::to_string(value);
      }
      
      static ::std::string IMAGE(sc_signal<INTEGER>& r) {
        return ::std::to_string(r.currentValue.value);
      }

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

    class BIT : public Enumeration<char> {
    public:
      using Enumeration<char>::operator=;
      using Enumeration<char>::IMAGE;
      
      static ::std::string IMAGE(sc_signal<BIT>& r) {
        return "'" + std::string(1, r.currentValue.value) + "'";
      }

      unsigned int LENGTH();
    };

    enum BOOLEAN_enum {FALSE, TRUE};
    
    struct BOOLEAN : public Enumeration<enum BOOLEAN_enum> {
    public:
      using Enumeration<enum BOOLEAN_enum>::operator=;
      using Enumeration<enum BOOLEAN_enum>::operator==;
      using Enumeration<enum BOOLEAN_enum>::operator!=;
      using Enumeration<enum BOOLEAN_enum>::IMAGE;
      
      std::string toString() {
        return (value == TRUE) ? "true" : "false";
      }
      
      static ::std::string IMAGE(bool r) {
        return r ? "true" : "false";
      }

      static ::std::string IMAGE(sc_signal<BOOLEAN>& r) {
        return IMAGE(r.currentValue.value == TRUE);
      }

      BOOLEAN();
      
      BOOLEAN(bool v);

      BOOLEAN operator!();
      
      operator bool() const;
      
      unsigned int LENGTH();
      
    };

    enum SEVERITY_LEVEL {NOTE, WARNING, ERROR, FAILURE};

    ::std::string toString(SEVERITY_LEVEL severity);
    
    enum TIME_UNITS {FS, PS, NS, US, MS, SEC, MIN, HR};

    /*
    class TIME : public Physical<int, TIME_UNITS> {
    public:
      explicit TIME(int value=0, TIME_UNITS units=NS) : Physical<int, TIME_UNITS>(value, units) {};
    };
    */

    typedef Physical<int, TIME_UNITS> TIME;

    extern TIME NOW;

    static TIME operator+(TIME left, const TIME& right);
    
    ::std::string toString(TIME_UNITS units);
    
  }

  void report(::std::string message, STANDARD::SEVERITY_LEVEL severity);

  static bool equal(STANDARD::INTEGER left, int right) {
    return left.value == right;
  }

}

#endif

