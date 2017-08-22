
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_type.hpp"
#include "vhdl_array.hpp"
#include "vhdl_enum.hpp"

namespace vhdl {
  
  /*
    VHDL functions
   */
  
  
  /*
    Expression operator functions
   */
  
  static ::std::string concat(::std::string left, ::std::string right) {
    return left + right;
  }

}

#endif

