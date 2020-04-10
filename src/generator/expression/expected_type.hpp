#ifndef _GENERATOR_EXPECTED_TYPE_HPP
#define _GENERATOR_EXPECTED_TYPE_HPP

#include "../../ast/object_type.hpp"
#include "../../debug/debug.hpp"

class ExpectedType {

  bool m_is_array;
  ast::ObjectTypes m_types;

public:
  ExpectedType(ast::ObjectValue x) : m_types(x) { m_is_array = (x == ast::ObjectValue::ARRAY); };
  ExpectedType(ast::ObjectValueContainer& x) : m_types(x) { m_is_array = x.isArray(); };
  std::string toString(bool verbose = false) { return m_types.toString(verbose); }
  bool equals(const ast::ObjectValueContainer& x) { return m_types.equals(x); }
  bool isUnique() { return m_types.isUnique(); }
  bool isArray() { return m_is_array; }
  void nextParenthisHierarchy() { if (m_types.isArray()) { m_types.nextSubtype();} }
  bool equalsExact(const ast::ObjectValueContainer& x) { return m_types.equalsExact(x); }
};

#endif
