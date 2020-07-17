#ifndef _GENERATOR_EXPECTED_TYPE_HPP
#define _GENERATOR_EXPECTED_TYPE_HPP

#include "../../ast/object_type.hpp"
#include "../../debug/debug.hpp"

namespace generator {

  class ExpectedType {

    Debug<false> m_debug;
  protected:
    bool m_is_array;
    ast::ObjectTypes m_types;

  public:
    ExpectedType() : m_debug(this) {}
    ExpectedType(ast::ObjectValue x) : m_debug(this), m_types(x) { m_is_array = (x == ast::ObjectValue::ARRAY); };
    ExpectedType(ast::ObjectValueContainer& x) : m_debug(this), m_types(x) { m_is_array = x.isArray(); };
    bool equals(const ast::ObjectValueContainer& x) { return m_types.equals(x); }
    bool isUnique() { return m_types.isUnique(); }
    bool isArray() { return m_is_array; }
    void subtype(ExpectedType& e);
    bool equalsExact(const ast::ObjectValueContainer& x) { return m_types.equalsExact(x); }
    std::string toString(bool verbose = false);
  };

}

#endif
