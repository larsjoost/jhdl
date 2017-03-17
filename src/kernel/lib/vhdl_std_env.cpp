#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_std_env.hpp"

namespace vhdl {

  namespace STD {

    void ENV::FINISH(int x) {
      std::cout << "Finish" << std::endl;
      sc_exit(x);
    }

    struct ENV ENV;
    
  }
  
}
