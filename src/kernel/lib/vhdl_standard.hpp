
#ifndef VHDL_STANDARD_H
#define VHDL_STANDARD_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <limits>

#include "systemc.h"
#include "vhdl_type.hpp"

namespace vhdl {

  namespace STANDARD {

    struct INTEGER_range {
      int left = std::numeric_limits<int>::min();
      int right = std::numeric_limits<int>::max();
    };

    template <class RANGE = INTEGER_range>
    using INTEGER = Range<int, RANGE>;
    
    struct NATURAL_range {
      int left = 0;
      int right = std::numeric_limits<int>::max();
    };
    template <class RANGE = NATURAL_range>
    using NATURAL = INTEGER<RANGE>;
    
    struct POSITIVE_range {
      int left = 1;
      int right = std::numeric_limits<int>::max();
    };
    template <class RANGE = POSITIVE_range>
    using POSITIVE = INTEGER<RANGE>;

    struct REAL_range {
      double left = std::numeric_limits<double>::max();
      double right = std::numeric_limits<double>::max();
    };
    template <class RANGE = REAL_range>
    using REAL = Range<double, RANGE>;
    
    char BIT_char[] = {'0', '1'};
    using BIT = CharArray<BIT_char>;

    enum BOOLEAN_enum {FALSE, TRUE};
    char* BOOLEAN_string[] = {(char *)"false", (char *)"true"};
    template<typename T = BOOLEAN_enum, char* p[] = BOOLEAN_string>
    using BOOLEAN = Enumeration<T, p>;

    enum SEVERITY_LEVEL_enum {NOTE, WARNING, ERROR, FAILURE};
    char* SEVERITY_LEVEL_string[] = {(char *)"note", (char *)"warning", (char *)"error", (char *)"failure"};
    using SEVERITY_LEVEL = Enumeration<enum SEVERITY_LEVEL_enum, SEVERITY_LEVEL_string>;

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

  void report(::std::string message, STANDARD::SEVERITY_LEVEL_enum severity);

}

#endif
