#ifndef VHDL_TYPE_HPP
#define VHDL_TYPE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>

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
    
    Range<TYPE>() {}
    Range<TYPE>(TYPE left, TYPE right) {
      a_left = left;
      a_right = right;
    }
    Range<TYPE>(TYPE value) {
      a_value = value;
    }
    
    void init(const Range<TYPE>& other) {
      a_left = other.a_left;
      a_right = other.a_right;
    };
    
    void operator=(const TYPE other) { a_value = other; }
    void operator=(const Range<TYPE>& other) { a_value = other.a_value; }

    bool operator ==(const Range<TYPE> &other) const { return a_value == other.a_value; }
    bool operator !=(const Range<TYPE> &other) const { return a_value != other.a_value; }
    bool operator ==(const TYPE other) const { return a_value == other; }
    bool operator <=(const TYPE other) const { return a_value <= other; }
    bool operator >=(const TYPE other) const { return a_value >= other; }
    bool operator !=(const TYPE other) const { return a_value != other; }
    TYPE operator +(const TYPE other) const { return a_value + other; }
    TYPE operator -(const TYPE other) const { return a_value - other; }
    TYPE operator +(const Range<TYPE>& other) const { return a_value + other.a_value; }
    TYPE operator -(const Range<TYPE>& other) const { return a_value - other.a_value; }
    
    operator bool() const {
      return a_value != TYPE(0);
    }
    
    operator int() const {
      return a_value;
    }

    std::string toString() {
      return std::to_string(a_value);
    }

    unsigned int LENGTH() {
      return 32;
    }

    TYPE HIGH() {
      return (a_left > a_right ? a_left : a_right);
    }
    TYPE LOW() {
      return (a_left < a_right ? a_left : a_right);
    }
    TYPE LEFT() {
      return a_left;
    }
    TYPE RIGHT() {
      return a_right;
    }
    TYPE LEFTOF() {
      return (a_left < a_right ? getValue() - 1 : getValue() + 1);
    }
    TYPE RIGHTOF() {
      return (a_left < a_right ? getValue() + 1 : getValue() - 1);
    }
		       
    int getValue() {
      return a_value;
    }

    std::string STATUS() {
      return toString();
    }
    
    template <class T>
    static ::std::string IMAGE(T& r) {
      return r.toString();
    }

    static ::std::string IMAGE(TYPE r) {
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
    
    void init(const PhysicalType<VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
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

}

#endif
