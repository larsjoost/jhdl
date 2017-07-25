#ifndef _STANDARD_H
#define _STANDARD_H

#include "standard.hpp"

namespace vhdl {

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity);

  STD::STANDARD::TIME_enum convert(SC_UNITS u);
  
  template<class RANGE>
  bool wait(STD::STANDARD::TIME<RANGE> t) {
    STD::STANDARD s;
    return (s.NOW() == t);
  };

  bool wait(STD::STANDARD::DELAY_LENGTH<> t);
  
}

#endif
