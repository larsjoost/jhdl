
#ifndef VHDL_H
#define VHDL_H

#include <cstdlib>
#include <iostream>

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
    T left;
    T right;
    T value;
  public:
    Range(T left, T right) {
      value = left;
      this->left = left;
      this->right = right;
    };

    void operator=(T v) {
      value = v;
    }
  };
    
  
}

#endif
