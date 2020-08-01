
#ifndef VHDL_ALIAS_HPP
#define VHDL_ALIAS_HPP

#include "../../debug/debug.hpp"

namespace vhdl {

  template<typename T>
  class Alias {
    Debug<false> m_debug;

    T& m_value;
    T m_type;
  public:
    Alias(T& value) : m_debug(this), m_value(value) {}

    template <typename T1, typename T2, typename T3>
    inline void constrain(T1 left, T2 right, T3 ascending) {
      m_type.constrain(left, right, ascending);
    }
    
    template <typename T1>
    inline auto& operator[](T1 index) {
      m_debug.functionStart("operator[]", false, __FILE__, __LINE__);
      int i = m_type.getAbsoluteIndex(index);
      m_debug.debug("i = " + std::to_string(i));
      auto& result = m_value.getAbsolute(i);
      m_debug.functionEnd("operator[]: " + result.info());
      return result;
    }

    
  };

}

#endif

