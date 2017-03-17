
#ifndef VHDL_STD_ENV_H
#define VHDL_STD_ENV_H

#include <iostream>
#include <string>

namespace vhdl {

  namespace STD {

    struct ENV {
      void FINISH(int x);
    };

    extern ENV ENV;
    
  }
  
}

#endif
