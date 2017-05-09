#ifndef _STANDARD_H
#define _STANDARD_H

#include "standard.hpp"

namespace vhdl {

  using namespace STD;
  
  void report(::std::string message, STANDARD::SEVERITY_LEVEL_enum severity);
  
}

#endif
