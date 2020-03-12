#include <csignal>

#include "debug_debug.hpp"

namespace DEBUG {
  
  void Package_DEBUG::vhdl_breakpoint() {
    std::raise(SIGINT);
  }
  
}
