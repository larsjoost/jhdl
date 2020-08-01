
#ifndef VHDL_PROCESS_HPP
#define VHDL_PROCESS_HPP

#include "../../debug/debug.hpp"

#include "vhdl_object.hpp"

namespace vhdl {

  class Process : public Object {
    int m_id;
    public:
    Process(Object* parent) : Object(parent) {};
    void setProcessId() {
      assert(m_parent);
      m_id = m_parent->getNextObjectId();
    }
    inline int getProcessId() {
      return m_id;
    }
  };

}

#endif

