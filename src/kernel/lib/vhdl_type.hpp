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
   * Range examples:
   *     
   * type INTEGER is range -2147483647 to 2147483647;
   * type REAL is range -1.7014111e+308 to 1.7014111e+308;
   */
  
  template<class TYPE>
  class Range {

  protected:

    TYPE m_value;
    TYPE m_left;
    TYPE m_right;
    bool m_ascending;

  public:
    
    Range<TYPE>() {}
    Range<TYPE>(TYPE left, TYPE right, bool ascending = true) {
      m_left = left;
      m_right = right;
      m_ascending = ascending;
    }
    Range<TYPE>(TYPE value) {
      m_value = value;
    }
    
    inline void construct(const Range<TYPE>& other) {
      m_left = other.m_left;
      m_right = other.m_right;
      m_ascending = other.m_ascending;
    };

    void init(TYPE value) {
      m_value = value;
    }
    
    inline void operator=(const TYPE other) { m_value = other; }
    inline void operator=(const Range<TYPE>& other) { m_value = other.m_value; }

    inline bool operator == (const Range<TYPE> &other) const { return m_value == other.m_value; }
    inline bool operator != (const Range<TYPE> &other) const { return m_value != other.m_value; }
    inline bool operator == (const TYPE other) const { return m_value == other; }
    inline bool operator <= (const TYPE other) const { return m_value <= other; }
    inline bool operator >= (const TYPE other) const { return m_value >= other; }
    inline bool operator != (const TYPE other) const { return m_value != other; }
    inline TYPE operator +  (const TYPE other) const { return m_value + other; }
    inline TYPE operator -  (const TYPE other) const { return m_value - other; }
    inline TYPE operator +  (const Range<TYPE>& other) const { return m_value + other.m_value; }
    inline TYPE operator -  (const Range<TYPE>& other) const { return m_value - other.m_value; }

    /* Required by sc_signals */
    inline friend std::ostream& operator << ( std::ostream& os,  Range<TYPE> const &other) {
      os << other.toString();
      return os;
    }

    inline friend void sc_trace(sc_trace_file *tf, const Range<TYPE> & v,
    const std::string & NAME ) {
      sc_trace(tf,v.m_value, NAME + ".value");
    }
    
    inline operator bool() const {
      return m_value != TYPE(0);
    }
    
    inline operator int() const {
      return m_value;
    }

    inline int ToInt() {
      return m_value;
    }
    
    inline const std::string toString() const {
      return std::to_string(m_value);
    }

    inline unsigned int LENGTH() {
      return 32;
    }

    inline TYPE HIGH() {
      return (m_left > m_right ? m_left : m_right);
    }
    inline TYPE LOW() {
      return (m_left < m_right ? m_left : m_right);
    }
    inline TYPE LEFT() {
      return m_left;
    }
    inline TYPE RIGHT() {
      return m_right;
    }
    inline TYPE LEFTOF() {
      return (m_left < m_right ? getValue() - 1 : getValue() + 1);
    }
    inline TYPE RIGHTOF() {
      return (m_left < m_right ? getValue() + 1 : getValue() - 1);
    }

    inline TYPE POS() {
      return m_value;
    }
    
    inline int getValue() {
      return m_value;
    }

    inline std::string STATUS() {
      return toString();
    }
    
    template <class T>
    std::string IMAGE(T& r) {
      return r.toString();
    }

    inline std::string IMAGE(TYPE r) {
      return std::to_string(r);
    }
    
  };

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
