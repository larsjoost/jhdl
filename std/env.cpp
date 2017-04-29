#include "env.hpp"

namespace vhdl {
  
  void vhdl_stop(STANDARD::INTEGER<> STATUS) {
    exit(STATUS);
  }
  
  void vhdl_finish(STANDARD::INTEGER<> STATUS) {
    exit(STATUS);
  }

}
