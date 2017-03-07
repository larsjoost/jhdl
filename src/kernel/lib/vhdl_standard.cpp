#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "vhdl_standard.hpp"
#include "vhdl_std_env.hpp"

namespace vhdl {
  namespace STANDARD {

    /*
    class TIME : public Physical<int, TIME_UNITS> {
    public:
      explicit TIME(int value=0, TIME_UNITS units=NS) : Physical<int, TIME_UNITS>(value, units) {};
    };
    */

    TIME NOW = TIME(0, NS);

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

  void report(::std::string message, STANDARD::SEVERITY_LEVEL_enum severity) {
    std::ostream* o;
    if (severity == STANDARD::ERROR || severity == STANDARD::FAILURE) {
      o = &std::cerr;
    } else {
      o = &std::cout;
    } 
    *o << STANDARD::SEVERITY_LEVEL::IMAGE(severity) << ": " << message << ::std::endl;
    if (severity == STANDARD::FAILURE) {
      STD::ENV::FINISH(1);
    } 
  }

}
