
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

  template<class T>
  class Range {
  public:
    Range(T left, T right) {};
  };
    
  
}

#endif
