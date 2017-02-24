#include <iostream>
#include <string>

#include "systemc.h"
#include "vhdl_std_env.hpp"

namespace vhdl {

  namespace STD {
    namespace ENV {

      void FINISH(int x) {
        std::cout << "Finish" << std::endl;
        sc_exit(x);
      }
      
    }
  }
  
}
