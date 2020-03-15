#include <iostream>

#include <systemc.h>

#include "std_env.hpp"

#include "../src/output/output.hpp"

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
  
  void report(std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity, const char* file_name, int line_number) {
    bool use_systemc = false;
    std::ostream* o = &std::cout;
    STD::STANDARD::SEVERITY_LEVEL s = severity;
    std::string text = "#[" + s.IMAGE(s) + "] " + std::string(file_name) + "(" + std::to_string(line_number) +  "): " + message;
    Output::Color color;
    switch (severity) {
    case STD::STANDARD::SEVERITY_LEVEL_enum::NOTE: {
      if (use_systemc) {SC_REPORT_INFO("vhdl::report", text.c_str());}
      color = Output::Color::BLUE;
      break;
    }
    case STD::STANDARD::SEVERITY_LEVEL_enum::WARNING: {
      if (use_systemc) {SC_REPORT_WARNING("vhdl::report", text.c_str());}
      color = Output::Color::YELLOW;
      break;
    }
    case STD::STANDARD::SEVERITY_LEVEL_enum::ERROR: 
      if (use_systemc) {SC_REPORT_ERROR("vhdl::report", text.c_str());}
      color = Output::Color::RED;
      o = &std::cerr;
      break;
    case STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE: {
      if (use_systemc) {SC_REPORT_FATAL("vhdl::report", text.c_str());}
      color = Output::Color::RED;
      o = &std::cerr;
    }
    }
    if (!use_systemc) {
      Output output(*o);
      output.println(color, text);
      *o << std::flush;
      if (severity == STD::STANDARD::SEVERITY_LEVEL_enum::FAILURE) {
	STD_ENV.FINISH(1);
      }
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
  
  
