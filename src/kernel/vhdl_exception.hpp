#ifndef VHDL_EXCEPTION_HPP
#define VHDL_EXCEPTION_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include <exception>

#include <systemc.h>

namespace vhdl {

  class RuntimeError : public std::exception {
  };
  
}

#endif
