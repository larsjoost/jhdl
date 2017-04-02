#include <iostream>

#include "standard.hpp"

namespace vhdl {

  void report(::std::string message, STANDARD::SEVERITY_LEVEL_enum severity) {
    std::ostream* o = &std::cout;
    if (severity == STANDARD::ERROR || severity == STANDARD::FAILURE) {
      o = &std::cerr;
    } 
    STANDARD::SEVERITY_LEVEL s;
    s = severity;
    *o << STANDARD::SEVERITY_LEVEL::IMAGE(s) << ": " << message << ::std::endl;
    if (severity == STANDARD::FAILURE) {
      STD::ENV.FINISH(1);
    } 
  }
  
}
  
