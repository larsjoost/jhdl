#include <iostream>

#include <systemc.h>

#include "std_env.hpp"

namespace vhdl {

  bool wait(const STD::STANDARD::TIME& t) {
    STD::STANDARD s;
    return (s.NOW() == t);
  };

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity) {
    std::ostream* o = &std::cout;
    if (severity == STD::STANDARD::SEVERITY_LEVEL_enum::ERROR ||
	severity == STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE) {
      o = &std::cerr;
    } 
    STD::STANDARD::SEVERITY_LEVEL s = severity;
    *o << s.IMAGE(s) << ": " << message << ::std::endl;
    if (severity == STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE) {
      STD_ENV.FINISH(1);
    } 
  }

}

namespace STD {

  Package_STANDARD::DELAY_LENGTH Package_STANDARD::vhdl_now() {
    sc_time x = sc_time_stamp();
    DELAY_LENGTH t = {(int)(x.to_seconds() * 1.0e15), STD::STANDARD::FS};
    return t;
  }

}
  
  
