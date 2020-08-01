
#ifndef VHDL_OBJECT_HPP
#define VHDL_OBJECT_HPP

#include "../../debug/debug.hpp"

namespace vhdl {

  class Object {
  protected:
    Object* m_parent = NULL;
  public:
    Object(Object* parent) : m_parent(parent) {};
    virtual int getNextObjectId() {
      assert(m_parent);
      return m_parent->getNextObjectId();
    }
  };

}

#endif

