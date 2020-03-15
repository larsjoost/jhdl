#include "std_env.hpp"

namespace STD {
  
  void Package_ENV::vhdl_stop(STANDARD::INTEGER STATUS) {
    // std::cout << "Stop" << std::endl;
    exit(STATUS);
  }
  
  void Package_ENV::vhdl_finish(STANDARD::INTEGER STATUS) {
    bool use_systemc = false;
    if (!use_systemc) {
      exit(STATUS.getValue());
    } else {
      if (STATUS.getValue() == 0) {
	sc_stop();
      } else {
	std::string m = "status = " + std::to_string(STATUS.getValue());
	SC_REPORT_FATAL("vhdl_finish", m.c_str());
      }
    }
  }

  STANDARD::DELAY_LENGTH Package_ENV::vhdl_resolution_limit() {
    return STANDARD::DELAY_LENGTH(1, STANDARD::FS);
  }

}
