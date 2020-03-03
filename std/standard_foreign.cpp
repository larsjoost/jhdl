#include <iostream>

#include <systemc.h>

#include "std_env.hpp"

namespace vhdl {
  
  sc_time convert(STD::STANDARD::TIME t) {
    double value = (double)t.getValue();
    switch (t.getUnit()) {
    case STD::STANDARD::TIME_enum::FS:
      return sc_time(value, SC_FS);
    case STD::STANDARD::TIME_enum::PS:
      return sc_time(value, SC_PS);
    case STD::STANDARD::TIME_enum::NS:
      return sc_time(value, SC_NS);
    case STD::STANDARD::TIME_enum::US:
      return sc_time(value, SC_US);
    case STD::STANDARD::TIME_enum::MS:
      return sc_time(value, SC_MS);
    case STD::STANDARD::TIME_enum::SEC:
      return sc_time(value, SC_SEC);
    case STD::STANDARD::TIME_enum::MIN:
      return sc_time(value * 60.0, SC_SEC);
    case STD::STANDARD::TIME_enum::HR:
      return sc_time(value * 60.0 * 60.0, SC_SEC);
    }
    assert(false);
  }
  
  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity, const char* file_name, int line_number) {
    std::ostream* o = &std::cout;
    if (severity == STD::STANDARD::SEVERITY_LEVEL_enum::ERROR ||
	severity == STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE) {
      o = &std::cerr;
    } 
    STD::STANDARD::SEVERITY_LEVEL s = severity;
    *o << "#[" << s.IMAGE(s) << "] " << file_name << "(" << line_number << "): " << message << ::std::endl;
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
  
  
