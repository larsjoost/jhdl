#ifndef _STANDARD_H
#define _STANDARD_H

#include <systemc.h>

#include "std_standard.hpp"

namespace vhdl {

  sc_time convert(STD::STANDARD::TIME t);

  void report(::std::string message, STD::STANDARD::SEVERITY_LEVEL_enum severity);

}

#endif
