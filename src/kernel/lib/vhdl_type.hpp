#ifndef VHDL_TYPE_HPP
#define VHDL_TYPE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>

namespace vhdl {

  /*
   * Range examples:
   *     
   * type INTEGER is range -2147483647 to 2147483647;
   * type REAL is range -1.7014111e+308 to 1.7014111e+308;
   */
  
  template<class TYPE, class RANGE>
  class Range {
  public:
    RANGE range;
    TYPE value;

    class iterator {
      friend class Range;
      Range<TYPE, RANGE>* parent;
      TYPE index;
    public:
      Range<TYPE, RANGE> operator *() const { parent->value = index; return *parent; }
      const iterator &operator ++() { ++index; return *this; }
      Range<TYPE, RANGE> operator ++(TYPE) { iterator copy(*this); ++index; return copy; }
      
      bool operator ==(const iterator &other) const { return index == other.index; }
      bool operator !=(const iterator &other) const { return index != other.index; }

    protected:
      iterator(Range<TYPE, RANGE>* parent, TYPE start) : parent(parent), index(start) { }
    };
    
  public:

    explicit Range<TYPE, RANGE>() : begin_(this, range.left), end_(this, range.right + 1) { }

    Range<TYPE, RANGE>(TYPE v) :
    value(v), begin_(this, range.left), end_(this, range.right + 1) { }

    template<class T>
    Range<TYPE, RANGE>(Range<TYPE, T>& other) : value(other.value),
      begin_(this, range.left), end_(this, range.right + 1) {};
    
    void operator=(const TYPE other) { value = other; }
    template <class T>
    void operator=(const Range<TYPE, T>& other) { value = other.value; }

    template <class T>
    bool operator ==(const Range<TYPE, T> &other) { return value == other.value; }
    template <class T>
    bool operator !=(const Range<TYPE, T> &other) { return value != other.value; }
    bool operator ==(TYPE other) { return value == other; }
    bool operator <=(TYPE other) { return value <= other; }
    bool operator >=(TYPE other) { return value >= other; }
    bool operator !=(TYPE other) { return value != other; }
    TYPE operator +(TYPE other) { return value + other; }
    TYPE operator -(TYPE other) { return value - other; }
    template <class T>
    TYPE operator +(const Range<TYPE, T>& other) { return value + other.value; }
    template <class T>
    TYPE operator -(const Range<TYPE, T>& other) { return value - other.value; }
    
    operator bool() const {
      return value != TYPE(0);
    }
    
    std::string toString() {
      return std::to_string(value);
    }

    unsigned int LENGTH() {
      return 32;
    }

    static TYPE HIGH() {
      RANGE range;
      return (range.left > range.right ? range.left : range.right);
    }
    static TYPE LOW() {
      RANGE range;
      return (range.left < range.right ? range.left : range.right);
    }
    static TYPE LEFT() {
      RANGE range;
      return range.left;
    }
    static TYPE RIGHT() {
      RANGE range;
      return range.right;
    }
    template <class R>
    static TYPE LEFTOF(Range<TYPE, R> x) {
      RANGE range;
      return (range.left < range.right ? x.getValue() - 1 : x.getValue() + 1);
    }
    template <class R>
    static TYPE RIGHTOF(Range<TYPE, R> x) {
      RANGE range;
      return (range.left < range.right ? x.getValue() + 1 : x.getValue() - 1);
    }
		       
    int getValue() {
      return value;
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

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    iterator begin_;
    iterator end_;

  };
  /*
   * vhdl_range_type examples:
   *
   * type INTEGER is range -2147483647 to 2147483647;
   * type REAL is range -1.7014111e+308 to 1.7014111e+308;
   */
#define vhdl_range_type(name, leftValue, rightValue)              \
  using type_##name = decltype(leftValue);                        \
  struct range_##name {                                           \
    type_##name left = leftValue;                                 \
    type_##name right = rightValue;                               \
  };                                                              \
  template <class RANGE = range_##name>                           \
  using name = Range<type_##name, RANGE>


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
    Physical(VALUE value, UNIT unit) :
      value(value), unit(unit) { }
  };

  template<class RANGE, typename VALUE, typename UNIT, class ELEMENTS, class UNIT_STRING_CONVERTER>
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
    VALUE value;
    UNIT unit;
    PhysicalType() {
      RANGE range;
      value = range.left.value;
      unit = PhysicalType<RANGE, VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>::getBaseUnit();
    };  
    PhysicalType(VALUE v, UNIT u) {value = v; unit = u;};
    
    template<class R>
    PhysicalType<RANGE, VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>
    operator +(PhysicalType<R, VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER> other) {
      PhysicalType<RANGE, VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER> p;
      UNIT lowestUnit = unit < other.unit ? unit : other.unit;
      p.value = scale(value, unit, lowestUnit) + scale(other.value, other.unit, lowestUnit);
      p.unit = lowestUnit;
      return p;
    }
    template<class R>
    void operator=(const PhysicalType<R, VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      value = other.value;
      unit = other.unit;
    }

    template<class R>
    bool operator==(const PhysicalType<R, VALUE, UNIT, ELEMENTS, UNIT_STRING_CONVERTER>& other) {
      return (value == other.value && unit == other.unit);
    }

    static UNIT getBaseUnit() {
      ELEMENTS e;
      return e.array[0].base;
    }

    static UNIT getHighUnit() {
      ELEMENTS e;
      return e.array[e.size - 1].base;
    }
    
    static Physical<VALUE, UNIT> HIGH() {
      RANGE range;
      VALUE max = (range.left.value > range.right.value) ? range.left.value : range.right.value;
      return {max, getHighUnit()};
    }
    static Physical<VALUE, UNIT> LOW() {return {1, getBaseUnit()};}

    template <class X>
    static ::std::string IMAGE(X r) {
      UNIT_STRING_CONVERTER u;
      return ::std::to_string(r.value) + " " + u.toString(r.unit);
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

  template <typename T, class E, int N>
  struct Enumeration {
    int* char_position_lookup() {
      const static E valueArray;
      const static int CHAR_TRANSLATE_SIZE =  256;
      int* a = new int[CHAR_TRANSLATE_SIZE];
      std::fill_n(a, CHAR_TRANSLATE_SIZE, -1);
      for (int i=0; i<valueArray.size; i++) {
        if (valueArray.array[i].c != 0) {
          a[valueArray.array[i].c] = i;
        }
      }
      return a;
    }

    int char_position(char c) {
      static int* l = char_position_lookup();
      return l[(int)c];
    }

  protected:
    void set(char c) {
      if (char_position(c) < 0) {
        std::cerr << "Assigned char value " << c << " not allowed" << std::endl;
      } else {
        value = char_position(c);
      }
    }
    
  public:
    int value = 0;

    void operator=(T v) {
      value = v;
    }
  
    void operator=(char c) {
      set(c);
    }

    bool operator!=(char c) {
      Enumeration<T, E, N> other;
      other.set(c);
      return value != other.value;
    }
    
    bool operator==(char c) {
      Enumeration<T, E, N> other = c;
      return value == other.value;
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

    std::string toString() {
      const static E valueArray;
      if (valueArray.array[value].c == 0) {
        return valueArray.array[value].s;
      }
      return "'" + std::string(1, valueArray.array[value].c) + "'";
    }

    int getValue() {
      return value;
    }
    
    template <class X>
    static ::std::string IMAGE(X& r) {
      return r.toString();
    }

    int LENGTH() { return 1; }
    
    bool operator ==(const Enumeration<T, E, N> &other) const { return value == other.value; }
    bool operator !=(const Enumeration<T, E, N> &other) const { return value != other.value; }
  };


  /*
   * Array example:
   *
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class TYPE, class RANGE>
  class Array {
  public:
    RANGE range;
    TYPE* value;

    class iterator {
      friend class Array;
      Array<TYPE, RANGE>* parent;
      int index;
    public:
      Array<TYPE, RANGE> operator *() const { parent->value = index; return *parent; }
      const iterator &operator ++() { ++index; return *this; }
      Array<TYPE, RANGE> operator ++(int) { iterator copy(*this); ++index; return copy; }
      
      bool operator ==(const iterator &other) const { return index == other.index; }
      bool operator !=(const iterator &other) const { return index != other.index; }

    protected:
      iterator(Array<TYPE, RANGE>* parent, int start) : parent(parent), index(start) { }
    };

    void init() {
      value = new TYPE[LENGTH()];
    }
    
  public:

    explicit Array<TYPE, RANGE>() : begin_(this, range.left), end_(this, range.right + 1) {
      init();
    }
    Array<TYPE, RANGE>(TYPE v) :
    value(v), begin_(this, range.left), end_(this, range.right + 1) {
      init();
    }
    
    void operator=(const TYPE other) { value = other; }
    template <class T>
    void operator=(const Array<TYPE, T>& other) { value = other.value; }
    void operator=(const char* other) {
      std::string s(other);
      assert(s.size() == LENGTH());
      int i = 0;
      for (auto c : s) {
        value[i++] = c;
      }
    }

    template <class T>
    bool operator ==(const Array<TYPE, T> &other) { return value == other.value; }
    template <class T>
    bool operator !=(const Array<TYPE, T> &other) { return value != other.value; }
    bool operator ==(TYPE other) { return value == other; }
    bool operator !=(TYPE other) { return value != other; }
    TYPE operator +(TYPE other) { return value + other; }
    TYPE operator -(TYPE other) { return value - other; }
    template <class T>
    TYPE operator +(const Array<TYPE, T>& other) { return value + other.value; }
    template <class T>
    TYPE operator -(const Array<TYPE, T>& other) { return value - other.value; }
    TYPE& operator [](int index) {
      int x = ASCENDING() ? 1 : -1;
      int i = x * index - range.left;
      assert(i >= 0 && i < LENGTH());
      return value[i];
    }
    
    std::string toString() {
      return std::to_string(value);
    }

    bool ASCENDING() {
      return (range.right > range.left ? true : false);
    }
    
    unsigned int LENGTH() { return abs(range.left - range.right) + 1; }
    TYPE& HIGH() { return (range.left > range.right ? LEFT() : RIGHT()); }
    TYPE& LOW() { return (range.left < range.right ? LEFT() : RIGHT()); }
    TYPE& LEFT() { return value[0]; }
    TYPE& RIGHT() { return value[LENGTH() - 1]; }
    
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

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

  private:
    iterator begin_;
    iterator end_;

  };
  
  /*
   * vhdl_array_type examples:
   *
   * type a_t is array (0 to 4) of bit;
   */
  
#define vhdl_array_type(name, leftValue, rightValue, type)        \
  struct range_##name {                                           \
    int left = leftValue;                                         \
    int right = rightValue;                                       \
  };                                                              \
  template <class RANGE = range_##name>                           \
  using name = Array<type, RANGE>

}

#endif
