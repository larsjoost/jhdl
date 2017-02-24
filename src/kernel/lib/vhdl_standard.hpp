
#ifndef VHDL_STANDARD_H
#define VHDL_STANDARD_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_type.hpp"

template<class T> class sc_signal;

namespace vhdl {

  namespace STANDARD {
  
    class INTEGER : public Range<int> {
    public:
      explicit INTEGER(int left=INT_MIN, int right=INT_MAX) : Range<int>(left, right) {};
      explicit INTEGER(Range<int> r) : Range<int>(r.left, r.right) {
        value = r.value;
      };
      using Range<int>::operator=;
      using Range<int>::IMAGE;
      static ::std::string IMAGE(sc_signal<INTEGER>& r);

    };
    
    static INTEGER operator+(INTEGER left, const int right);

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
      
      static ::std::string IMAGE(sc_signal<BIT>& r);

      unsigned int LENGTH();
    };

    enum BOOLEAN_enum {FALSE, TRUE};
    
    struct BOOLEAN : public Enumeration<enum BOOLEAN_enum> {
    public:
      using Enumeration<enum BOOLEAN_enum>::operator=;
      using Enumeration<enum BOOLEAN_enum>::IMAGE;
      
      static ::std::string IMAGE(sc_signal<BOOLEAN>& r);

      BOOLEAN();
      
      BOOLEAN(bool v);

      BOOLEAN operator!();
      
      operator bool() const;
      
      unsigned int LENGTH();
      
    };

    enum SEVERITY_LEVEL {NOTE, WARNING, ERROR, FAILURE};

    static const std::string SEVERITY_LEVEL_STRINGS[] =
      {"NOTE", "WARNING", "ERROR", "FAILURE"};
    
    ::std::string toString(SEVERITY_LEVEL severity);
    
    enum TIME_UNITS {FS, PS, NS, US, MS, SEC, MIN, HR};

    /*
    class TIME : public Physical<int, TIME_UNITS> {
    public:
      explicit TIME(int value=0, TIME_UNITS units=NS) : Physical<int, TIME_UNITS>(value, units) {};
    };
    */

    typedef Physical<int, TIME_UNITS> TIME;

    static TIME NOW;

    static TIME operator+(TIME left, const TIME& right);
    
    ::std::string toString(TIME_UNITS units);
    
  }

  void report(::std::string message, STANDARD::SEVERITY_LEVEL severity);

  static bool equal(STANDARD::INTEGER left, int right);

}

#endif

