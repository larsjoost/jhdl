#ifndef _STANDARD_H
#define _STANDARD_H

#include "std_standard.hpp"

namespace vhdl {

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity);

  bool wait(const STD::STANDARD::TIME& t);

  class Wait {
    STD::STANDARD::TIME timeout;
  public:
    int index = 0;
    void waitFor(const STD::STANDARD::TIME& t) {
      timeout = STD_STANDARD.NOW() + t;
    }
    bool done() {
      return STD_STANDARD.NOW() == timeout;
    }
  };
  
}

#endif
