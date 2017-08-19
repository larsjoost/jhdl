#include <iostream>

#include "standard.h"
#include "env.hpp"

namespace vhdl {
  
  STD::STANDARD STD_STANDARD;

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity) {
    std::ostream* o = &std::cout;
    if (severity == STD::STANDARD::SEVERITY_LEVEL_enum::ERROR ||
        severity == STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE) {
      o = &std::cerr;
    } 
    STD::STANDARD::SEVERITY_LEVEL s = severity;
    *o << STD::STANDARD::SEVERITY_LEVEL::IMAGE(s) << ": " << message << ::std::endl;
    if (severity == STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE) {
      STD::ENV e;
      e.FINISH(1);
    } 
  }

  STD::STANDARD::TIME_enum convert(SC_UNITS u) {
    switch (u) {
    case SC_FS : return STD::STANDARD::FS;
    case SC_PS : return STD::STANDARD::PS;
    case SC_NS : return STD::STANDARD::NS;
    case SC_US : return STD::STANDARD::US;
    case SC_MS : return STD::STANDARD::MS;
    case SC_SEC : return STD::STANDARD::SEC;
    case SC_MIN : return STD::STANDARD::MIN;
    case SC_HR : return STD::STANDARD::HR;
    }
    assert(false);
  };
 
  namespace STD {

    STANDARD::DELAY_LENGTH STANDARD::NOW() {
      STANDARD::DELAY_LENGTH t = {sc_now.value, convert(sc_now.unit)};
      return t;
    }

  }
  
}
  
