
#ifndef VHDL_ARCHITECTURE_HPP
#define VHDL_ARCHITECTURE_HPP

#include "../../debug/debug.hpp"

#include "vhdl_object.hpp"

namespace vhdl {

  class Architecture : public Object {
    int m_id = 0;
  public:
    Architecture() : Object(NULL) {};
    int getNextObjectId() {
      return m_id++;
    }
  };

}

#endif

