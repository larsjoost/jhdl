#include "env.hpp"

namespace vhdl {
  namespace STD {
  
    void vhdl_stop(STANDARD::INTEGER<> STATUS) {
      // std::cout << "Stop" << std::endl;
      exit(STATUS);
    }
  
    void vhdl_finish(STANDARD::INTEGER<> STATUS) {
      // std::cout << "Finish" << std::endl;
      exit(STATUS);
    }

    STANDARD::DELAY_LENGTH<> vhdl_resolution_limit() {
      return STANDARD::DELAY_LENGTH<>(1, STANDARD::FS);
    }

  }
}
