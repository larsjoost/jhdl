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
  public:
    TYPE a_value;
    TYPE a_left;
    TYPE a_right;
    bool m_ascending;
    
    Range<TYPE>() {}
    Range<TYPE>(TYPE left, TYPE right, bool ascending = true) {
      a_left = left;
      a_right = right;
      m_ascending = ascending;
    }
    Range<TYPE>(TYPE value) {
      a_value = value;
    }
    
    inline void construct(const Range<TYPE>& other) {
      a_left = other.a_left;
      a_right = other.a_right;
      m_ascending = other.m_ascending;
    };

    void init(TYPE value) {
      a_value = value;
    }
    
    inline void operator=(const TYPE other) { a_value = other; }
    inline void operator=(const Range<TYPE>& other) { a_value = other.a_value; }

    inline bool operator == (const Range<TYPE> &other) const { return a_value == other.a_value; }
    inline bool operator != (const Range<TYPE> &other) const { return a_value != other.a_value; }
    inline bool operator == (const TYPE other) const { return a_value == other; }
    inline bool operator <= (const TYPE other) const { return a_value <= other; }
    inline bool operator >= (const TYPE other) const { return a_value >= other; }
    inline bool operator != (const TYPE other) const { return a_value != other; }
    inline TYPE operator +  (const TYPE other) const { return a_value + other; }
    inline TYPE operator -  (const TYPE other) const { return a_value - other; }
    inline TYPE operator +  (const Range<TYPE>& other) const { return a_value + other.a_value; }
    inline TYPE operator -  (const Range<TYPE>& other) const { return a_value - other.a_value; }

    /* Required by sc_signals */
    inline friend std::ostream& operator << ( std::ostream& os,  Range<TYPE> const &other) {
      os << other.toString();
      return os;
    }

    inline friend void sc_trace(sc_trace_file *tf, const Range<TYPE> & v,
    const std::string & NAME ) {
      sc_trace(tf,v.a_value, NAME + ".value");
    }
    
    inline operator bool() const {
      return a_value != TYPE(0);
    }
    
    inline operator int() const {
      return a_value;
    }

    inline int ToInt() {
      return a_value;
    }
    
    inline const std::string toString() const {
      return std::to_string(a_value);
    }

    inline unsigned int LENGTH() {
      return 32;
    }

    inline TYPE HIGH() {
      return (a_left > a_right ? a_left : a_right);
    }
    inline TYPE LOW() {
      return (a_left < a_right ? a_left : a_right);
    }
    inline TYPE LEFT() {
      return a_left;
    }
    inline TYPE RIGHT() {
      return a_right;
    }
    inline TYPE LEFTOF() {
      return (a_left < a_right ? getValue() - 1 : getValue() + 1);
    }
    inline TYPE RIGHTOF() {
      return (a_left < a_right ? getValue() + 1 : getValue() - 1);
    }

    inline TYPE POS() {
      return a_value;
    }
    
    inline int getValue() {
      return a_value;
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
    VALUE a_value;
    UNIT a_unit;
    Physical<VALUE, UNIT> a_left;
    Physical<VALUE, UNIT> a_right;
    
    PhysicalType() {
      a_value = 0;
      a_unit = PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>::getBaseUnit();
    };  
    PhysicalType(VALUE value, UNIT unit) {a_value = value; a_unit = unit;};
    PhysicalType(UNIT unit) {a_unit = unit;}
    PhysicalType(const Physical<VALUE, UNIT>& left, const Physical<VALUE, UNIT>& right) {
      a_left = left;
      a_right = right;
    }
    
    void construct(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      a_left = other.a_left;
      a_right = other.a_right;
    };

    PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>
    operator +(PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER> other) {
      PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER> p;
      UNIT lowest_unit = a_unit < other.a_unit ? a_unit : other.a_unit;
      p.a_value = scale(a_value, a_unit, lowest_unit) + scale(other.a_value, other.a_unit, lowest_unit);
      p.a_unit = lowest_unit;
      return p;
    }

    void operator=(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      a_value = other.a_value;
      a_unit = other.a_unit;
    }

    bool operator==(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      return (a_value == other.a_value && a_unit == other.a_unit);
    }

    static UNIT getBaseUnit() {
      ELEMENTS e;
      return e.array[0].base;
    }

    UNIT getUnit() {
      return a_unit;
    }

    VALUE getValue() {
      return a_value;
    }
    
    static UNIT getHighUnit() {
      ELEMENTS e;
      return e.array[e.size - 1].base;
    }
    
    Physical<VALUE, UNIT> HIGH() {
      VALUE max = (a_left.value > a_right.value) ? a_left.value : a_right.value;
      return {max, getHighUnit()};
    }
    Physical<VALUE, UNIT> LOW() {return {1, getBaseUnit()};}

    template <class X>
    static ::std::string IMAGE(X r) {
      UNIT_STRING_CONVERTER u;
      return ::std::to_string(r.a_value) + " " + u.toString(r.a_unit);
    }

  };

  template<class T>
  class vhdl_access {

    T a_value;

    bool a_null = true;
  
  public:

    T& ALL() {
      assert(!a_null);
      return a_value;
    }

    void set(T& s) {
      a_null = false;
      a_value.set(s);
    }

    void construct(vhdl_access<T> s) {
      a_value.construct(s.a_value);
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
