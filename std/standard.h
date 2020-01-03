#ifndef _STANDARD_H
#define _STANDARD_H

#include "standard.hpp"

namespace vhdl {

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity);

  bool wait(const STD::STANDARD::TIME& t);

  class Wait {
    STD::STANDARD s;
    STD::STANDARD::TIME timeout;
  public:
    int index = 0;
    void waitFor(const STD::STANDARD::TIME& t) {
      timeout = s.NOW() + t;
    }
    bool done() {
      return s.NOW() == timeout;
    }
  };
  
}

#endif
