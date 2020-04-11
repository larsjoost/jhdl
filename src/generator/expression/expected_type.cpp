
#include "expected_type.hpp"

namespace generator {

  void ExpectedType::subtype(ExpectedType& e) {
    m_debug.functionStart("subtype(e = " + e.toString(true) + ")", false, __FILE__, __LINE__);
    m_is_array = e.m_is_array;
    m_types.set(e.m_types);
    m_types.nextSubtype();
    m_debug.functionEnd("subtype: " + toString(true));
  }

  std::string ExpectedType::toString(bool verbose) {
    std::string result;
    if (verbose) {
      result = "m_is_array = " + std::string(m_is_array ? "true" : "false") + ", m_types = " + m_types.toString(verbose);
    } else {
      result = m_types.toString();
    }
    return result;
  }

}
