
#ifndef VHDL_RESOLVED_HPP
#define VHDL_RESOLVED_HPP

#include "../../debug/debug.hpp"

namespace vhdl {

  template<typename T>
  class Resolved {
    Debug<true> m_debug;

    T m_value;
  public:
    Resolved() : m_debug(this) {}

    template<typename Object>
    void assign(Object* object, auto value) {
      
    }
    
    
  };

}

#endif

