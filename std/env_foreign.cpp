#include "env.hpp"

namespace vhdl {
  namespace STD {
  
    void ENV::vhdl_stop(STANDARD::INTEGER STATUS) {
      // std::cout << "Stop" << std::endl;
      exit(STATUS);
    }
  
    void ENV::vhdl_finish(STANDARD::INTEGER STATUS) {
      // std::cout << "Finish" << std::endl;
      exit(STATUS);
    }

    STANDARD::DELAY_LENGTH ENV::vhdl_resolution_limit() {
      return STANDARD::DELAY_LENGTH(1, STANDARD::FS);
    }

  }
}
