#include "env.hpp"

namespace vhdl {
  namespace STD {
  
    void vhdl_stop(STANDARD::INTEGER<> STATUS) {
      exit(STATUS);
    }
  
    void vhdl_finish(STANDARD::INTEGER<> STATUS) {
      exit(STATUS);
    }

    STANDARD::DELAY_LENGTH<> vhdl_resolution_limit() {
      return STANDARD::DELAY_LENGTH<>(1, STANDARD::FS);
    }

  }
}
