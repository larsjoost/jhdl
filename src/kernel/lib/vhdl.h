
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_type.hpp"

namespace vhdl {
  
  /*
    VHDL functions
   */
  
  
  /*  
  void wait_for(STANDARD::TIME& time) {
    auto t = STANDARD::NOW + time;
    sc_wait(time.value);
    report("Waiting until " + STANDARD::TIME::IMAGE(t), STANDARD::NOTE);
  }
  */
  /*
    Expression operator functions
   */
  
  static ::std::string concat(::std::string left, ::std::string right) {
    return left + right;
  }

}

#endif

