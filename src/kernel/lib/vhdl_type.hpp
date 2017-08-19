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
  

  /*
   * Enumeration example:
   *
   * type SEVERITY_LEVEL is (NOTE, WARNING, ERROR, FAILURE);
   */

  template<typename T>
  struct EnumerationElement {
    T e;
    char c;
    std::string s;
  };

  template <typename T, class E>
  struct Enumeration {
    int* char_position_lookup() {
      const static E valueArray;
      const static int SIZE =  256;
      int* a = new int[SIZE];
      std::fill_n(a, SIZE, -1);
      for (int i=0; i<valueArray.size; i++) {
        if (valueArray.array[i].c != 0) {
          a[valueArray.array[i].c] = i;
        }
      }
      return a;
    }

    int underlying_index(T e) {
      return static_cast<std::underlying_type_t<T>>(e);
    }
    
    int* enum_position_lookup() {
      const static E valueArray;
      const static int SIZE =  valueArray.enum_size;
      int* a = new int[SIZE];
      for (int i=0; i<valueArray.size; i++) {
        if (valueArray.array[i].c == 0) {
          int index = underlying_index(valueArray.array[i].e);
          a[index] = i;
        }
      }
      return a;
    }

    int char_position(char c) {
      static int* l = char_position_lookup();
      return l[(int)c];
    }

    int enum_position(T e) {
      static int* l = enum_position_lookup();
      int index = underlying_index(e);
      return l[index];
    }
    
  protected:

    void set(char c) {
      if (char_position(c) < 0) {
        std::cerr << "Assigned char value " << c << " not allowed" << std::endl;
      } else {
        value = char_position(c);
      }
    }
    void set(T e) {
      value = enum_position(e);
    }
    
  public:
    int value = 0;

    Enumeration<T, E>() {
    }
    
    Enumeration<T, E>(T v) {
      set(v);
    }

    void operator=(T v) {
      set(v);
    }
  
    void operator=(char c) {
      set(c);
    }

    void operator=(bool v) {
      value = v ? (T)1 : (T)0;
    }

    bool operator!() {
      return value != (T)0;
    }
      
    operator bool() const {
      return value != (T)0;
    }

    std::string toString(bool with_quotes = true) {
      const static E valueArray;
      if (valueArray.array[value].c == 0) {
        return valueArray.array[value].s;
      }
      std::string s = with_quotes ? "'" : "";
      return s + std::string(1, valueArray.array[value].c) + s;
    }

    int POS() {
      return value;
    }

    int getValue() {
      return value;
    }
    
    template <class X>
    static ::std::string IMAGE(X& r) {
      return r.toString();
    }

    int LENGTH() { return 1; }
    
    bool operator==(T e) const {Enumeration<T, E> other; other.set(e); return value == other.value;}
    bool operator!=(T e) const {Enumeration<T, E> other; other.set(e); return value != other.value;}
    bool operator==(char c) const {Enumeration<T, E> other; other.set(c); return value == other.value;}
    bool operator!=(char c) const {Enumeration<T, E> other; other.set(c); return value != other.value;}
    bool operator ==(const Enumeration<T, E> &other) const { return value == other.value; }
    bool operator !=(const Enumeration<T, E> &other) const { return value != other.value; }
  };


  /*
   * Array example:
   *
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class TYPE>
  class Array {
  public:
    struct Range {
      int left;
      int right;
    };
    Range range;
    TYPE* value = NULL;

    void init(int left, int right) {
      range.left = left;
      range.right = right;
    }
    
    void init(const Array<TYPE>& other) {
      range = other.range;
    }

    void init() {
      assert(value == NULL);
      value = new TYPE[LENGTH()];
    }
    
    void set(const std::string& s) {
      assert(s.size() == LENGTH());
      if (value == NULL) { init(); }
      int i = 0;
      for (auto c : s) {
        value[i++] = c;
      }
    }

    int ConvertIndex(int index) {
      int x = ASCENDING() ? 1 : -1;
      return x * index - range.left;
    }

    bool IndexCheck(int i) {
      return i >= 0 && i < LENGTH();
    }
    
    bool WithinRange(int index) {
      int i = ConvertIndex(index);
      return IndexCheck(i);
    }
    
    TYPE& get(int index) {
      int i = ConvertIndex(index);
      assert(IndexCheck(i));
      return value[i];
    }
    
    Array<TYPE>() {
    }

    Array<TYPE>(int left, int right) {
      range.left = left;
      range.right = right;
    }
    
    explicit Array<TYPE>(int size) {
      resize(size);
      init();
    }
    
    void resize(int size) {
      assert(size > 0);
      range.right = ASCENDING() ? range.left + size - 1 : range.left - size + 1;
    }
    
    void set(Array<TYPE>& other) {
      if (value != NULL) {delete value; value = NULL; }
      range = other.range;
      init();
      for (int i = range.left; WithinRange(i); ASCENDING() ? i++ : i--) {
        get(i) = other[i];
      }
    }

    void operator=(const TYPE other) { value = other; }
    template <class T>
    void operator=(const char* other) {std::string s(other); set(s);}
    void operator=(const std::string other) {set(other);}

    TYPE& operator [](int index) {
      return get(index);
    }
    
    bool ASCENDING() {
      return (range.right > range.left ? true : false);
    }
    
    unsigned int LENGTH() { return abs(range.left - range.right) + 1; }
    TYPE& HIGH() { return (range.left > range.right ? LEFT() : RIGHT()); }
    TYPE& LOW() { return (range.left < range.right ? LEFT() : RIGHT()); }
    TYPE& LEFT() { return value[0]; }
    TYPE& RIGHT() { return value[LENGTH() - 1]; }

    std::string toString() {
      std::string s;
      for (int i = range.left; WithinRange(i); ASCENDING() ? i++ : i--) {
        s += this->get(i).toString(false);
      }
      return s;
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

}

#endif
