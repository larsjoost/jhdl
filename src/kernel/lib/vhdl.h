
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_std_env.hpp"
#include "vhdl_standard.hpp"

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

  template<typename T1, typename T2>
  static auto physical(T1 value, T2 units) -> Physical<T1, T2> {
    return Physical<T1, T2>(value, units);
  }

}

#endif

