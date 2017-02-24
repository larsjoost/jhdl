#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "vhdl_standard.hpp"

namespace vhdl {
  namespace STANDARD {
  
    static ::std::string INTEGER::IMAGE(sc_signal<INTEGER>& r) {
      return ::std::to_string(r.currentValue.value);
    }
    
    static INTEGER operator+(INTEGER left, const int right) {
      left.value += right;
      return left;
    }

    static ::std::string BIT::IMAGE(sc_signal<BIT>& r) {
      return "'" + std::string(1, r.currentValue.value) + "'";
    }

    unsigned int BIT::LENGTH() {
      return 1;
    }

    enum BOOLEAN_enum {FALSE, TRUE};
    
    static ::std::string BOOLEAN::IMAGE(sc_signal<BOOLEAN>& r) {
      return (r.currentValue.value == TRUE) ? "true" : "false";
    }

    BOOLEAN::BOOLEAN() {
      value = FALSE;
    }
      
    BOOLEAN::BOOLEAN(bool v) {
      value = v ? TRUE : FALSE;
    }

    BOOLEAN BOOLEAN::operator!() {
      BOOLEAN b;
      b.value = (value == TRUE ? FALSE : TRUE);
      return b;
    }
      
    BOOLEAN::operator bool() const {
      return value == TRUE;
    }

    unsigned int BOOLEAN::LENGTH() {
      return 1;
    }

    static const std::string SEVERITY_LEVEL_STRINGS[] =
      {"NOTE", "WARNING", "ERROR", "FAILURE"};
    
    ::std::string toString(SEVERITY_LEVEL severity); {
      return SEVERITY_LEVEL_STRINGS[severity];
    }
    
    /*
    class TIME : public Physical<int, TIME_UNITS> {
    public:
      explicit TIME(int value=0, TIME_UNITS units=NS) : Physical<int, TIME_UNITS>(value, units) {};
    };
    */

    static TIME NOW = TIME(0, NS);

    static TIME operator+(TIME left, const TIME& right) {
      left.value += right.value;
      return left;
    }
    
    ::std::string toString(TIME_UNITS units) {
      static const std::string TIME_UNITS_STRINGS[] =
        {"FS", "PS", "NS", "US", "MS", "SEC", "MIN", "HR"};
      return TIME_UNITS_STRINGS[units];
    }
    
  }

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

  static bool equal(STANDARD::INTEGER left, int right) {
    return left.value == right;
  }


}
