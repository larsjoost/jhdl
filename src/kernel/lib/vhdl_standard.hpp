
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

    vhdl_range_subtype(INTEGER, Range, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    vhdl_range_subtype(NATURAL, Range, 0, std::numeric_limits<int>::max());
    vhdl_range_subtype(POSITIVE, Range, 1, std::numeric_limits<int>::max());
    vhdl_range_subtype(REAL, Range, std::numeric_limits<double>::min(), std::numeric_limits<double>::max());

    char BIT_char[] = {'0', '1'};
    template <char* p = BIT_char>
    using BIT = CharArray<p>;

    enum BOOLEAN_enum {FALSE, TRUE};
    char* BOOLEAN_string[] = {(char *)"false", (char *)"true"};
    template<typename T = BOOLEAN_enum, char* p[] = BOOLEAN_string>
    using BOOLEAN = Enumeration<T, p>;

    enum SEVERITY_LEVEL_enum {NOTE, WARNING, ERROR, FAILURE};
    char* SEVERITY_LEVEL_string[] = {(char *)"note", (char *)"warning", (char *)"error", (char *)"failure"};
    using SEVERITY_LEVEL = Enumeration<enum SEVERITY_LEVEL_enum, SEVERITY_LEVEL_string>;

    enum TIME_UNITS {FS, PS, NS, US, MS, SEC, MIN, HR};
    char* TIME_UNITS_string[] =
      {(char *)"FS", (char *)"PS", (char *)"NS", (char *)"US", (char *)"MS", (char *)"SEC", (char *)"MIN", (char *)"HR"};
    template <typename Units = TIME_UNITS, char* p[] = TIME_UNITS_string>
    using TIME = Physical<int, Units, p>;

    /*
    class TIME : public Physical<int, TIME_UNITS> {
    public:
      explicit TIME(int value=0, TIME_UNITS units=NS) : Physical<int, TIME_UNITS>(value, units) {};
    };
    */

    TIME<> NOW(0, NS);

  }

  void report(::std::string message, STANDARD::SEVERITY_LEVEL_enum severity) {
    std::ostream* o = &std::cout;
    if (severity == STANDARD::ERROR || severity == STANDARD::FAILURE) {
      o = &std::cerr;
    } 
    STANDARD::SEVERITY_LEVEL s;
    s = severity;
    *o << STANDARD::SEVERITY_LEVEL::IMAGE(s) << ": " << message << ::std::endl;
    if (severity == STANDARD::FAILURE) {
      STD::ENV::FINISH(1);
    } 
  }

}

#endif

