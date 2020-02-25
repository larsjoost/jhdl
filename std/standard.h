#ifndef _STANDARD_H
#define _STANDARD_H

#include "std_standard.hpp"

namespace vhdl {

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity);

  void wait(STD::STANDARD::TIME t);
  
}

#endif
