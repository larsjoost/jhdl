#ifndef VHDL_TYPE_HPP
#define VHDL_TYPE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>

#include <systemc.h>

namespace vhdl {

  /*
   * Physical example:
   * 
   * 10 ns
   */

  template<typename T1, typename T2>
  struct PhysicalElement {
    T1 base;
    T2 number;
    T1 unit;
  };
  
  template <class VALUE, typename UNIT>
  struct Physical {
    VALUE value;
    UNIT unit;
  };

  template<typename VALUE, typename UNIT, class ELEMENTS, class UNIT_STRING_CONVERTER>
  class PhysicalType {
    VALUE scale(VALUE v, UNIT u, UNIT l) {
      ELEMENTS e;
      assert(l >= u);
      if (u == l) {
	return v;
      } 
      return scale(v * e.array[u].number, e.array[u+1].base, l);
    }
  public:
    VALUE m_value;
    UNIT a_unit;
    Physical<VALUE, UNIT> m_left;
    Physical<VALUE, UNIT> m_right;
    
    PhysicalType() {
      m_value = 0;
      a_unit = PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>::getBaseUnit();
    };  
    PhysicalType(VALUE value, UNIT unit) {m_value = value; a_unit = unit;};
    PhysicalType(UNIT unit) {a_unit = unit;}
    PhysicalType(const Physical<VALUE, UNIT>& left, const Physical<VALUE, UNIT>& right) {
      m_left = left;
      m_right = right;
    }
    
    void construct(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      m_left = other.m_left;
      m_right = other.m_right;
    };

    PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>
    operator +(PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER> other) {
      PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER> p;
      UNIT lowest_unit = a_unit < other.a_unit ? a_unit : other.a_unit;
      p.m_value = scale(m_value, a_unit, lowest_unit) + scale(other.m_value, other.a_unit, lowest_unit);
      p.a_unit = lowest_unit;
      return p;
    }

    void operator=(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      m_value = other.m_value;
      a_unit = other.a_unit;
    }

    bool operator==(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      return (m_value == other.m_value && a_unit == other.a_unit);
    }

    static UNIT getBaseUnit() {
      ELEMENTS e;
      return e.array[0].base;
    }

    UNIT getUnit() {
      return a_unit;
    }

    VALUE getValue() {
      return m_value;
    }
    
    static UNIT getHighUnit() {
      ELEMENTS e;
      return e.array[e.size - 1].base;
    }
    
    Physical<VALUE, UNIT> HIGH() {
      VALUE max = (m_left.value > m_right.value) ? m_left.value : m_right.value;
      return {max, getHighUnit()};
    }
    Physical<VALUE, UNIT> LOW() {return {1, getBaseUnit()};}

    template <class X>
    static ::std::string IMAGE(X r) {
      UNIT_STRING_CONVERTER u;
      return ::std::to_string(r.m_value) + " " + u.toString(r.a_unit);
    }

  };

  template<class T>
  class vhdl_access {

    T m_value;

    bool a_null = true;
  
  public:

    T& ALL() {
      assert(!a_null);
      return m_value;
    }

    void set(T& s) {
      a_null = false;
      m_value.set(s);
    }

    void construct(vhdl_access<T> s) {
      m_value.construct(s.m_value);
    }
  
    bool isNull() {
      return a_null;
    };

    void DEALLOCATE() {
      a_null = true;
    }

  };


  
  
}

#endif
