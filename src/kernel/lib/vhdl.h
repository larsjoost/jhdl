
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>

namespace vhdl {

  namespace std {
    namespace env {

      void finish(int x) {
        exit(x);
      }
      
    }
  }

  class Range {
  public:
    Range(int left, int right);
    Range(float left, float right);
  };
    
  
}

#endif
