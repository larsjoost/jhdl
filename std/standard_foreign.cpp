#include <iostream>

#include "standard.h"
#include "env.hpp"

namespace vhdl {
  using namespace STD;
  
  void report(::std::string message, STANDARD::SEVERITY_LEVEL_enum severity) {
    std::ostream* o = &std::cout;
    if (severity == STANDARD::ERROR || severity == STANDARD::FAILURE) {
      o = &std::cerr;
    } 
    STANDARD::SEVERITY_LEVEL<> s;
    s = severity;
    *o << STANDARD::SEVERITY_LEVEL<>::IMAGE(s) << ": " << message << ::std::endl;
    if (severity == STANDARD::FAILURE) {
      ENV e;
      e.FINISH(1);
    } 
  }

  namespace STD {

    STANDARD::BOOLEAN<> vhdl_not_equal(STANDARD::INTEGER<> A, STANDARD::INTEGER<> B) {
      return A != B;
    }

    STANDARD::STRING<> vhdl_concat(STANDARD::STRING<> A, STANDARD::STRING<> B) {
      return A + B;
    }
  }
  
}
  
