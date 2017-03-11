#ifndef VHDL_TYPE_HPP
#define VHDL_TYPE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

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
    
    void operator=(const TYPE other) { value = other; }
    template <class T>
    void operator=(const Range<TYPE, T>& other) { value = other.value; }

    template <class T>
    bool operator ==(const Range<TYPE, T> &other) { return value == other.value; }
    template <class T>
    bool operator !=(const Range<TYPE, T> &other) { return value != other.value; }
    bool operator ==(TYPE other) { return value == other; }
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

    TYPE HIGH() { return (range.left > range.right ? range.left : range.right); }
    TYPE LOW() { return (range.left < range.right ? range.left : range.right); }
    TYPE LEFT() { return range.left; }
    TYPE RIGHT() { return range.right; }
    
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
   * vhdl_range_subtype examples:
   *
   * subtype NATURAL is INTEGER range 0 to INTEGER'HIGH;
   */
  
#define vhdl_range_subtype(name, subtype, leftValue, rightValue)  \
  using type_##name = decltype(leftValue);                        \
  struct range_##name {                                           \
    type_##name left = leftValue;                                 \
    type_##name right = rightValue;                               \
  };                                                              \
  template <class RANGE = range_##name>                           \
  using name = subtype<RANGE>

  /*
   * Physical example:
   * 
   * 10 ns
   */
  
  template <class T, typename Units, char* p[]>
  class Physical {
  protected:
  public:
    T value;
    Units units;

    Physical() {};
    
    Physical(T value, Units units) :
      value(value), units(units) { }

    Physical<T, Units, p> operator+(const Physical<T, Units, p>& other) {
      Physical<T, Units, p> r;
      r.value = value + other.value;
      return r;
    }
    
    static ::std::string IMAGE(Physical<T, Units, p>& r) {
      return std::to_string(r.value) + " " + p[r.units];
    }

  };

  /*
   * Enumeration example:
   *
   * type SEVERITY_LEVEL is (NOTE, WARNING, ERROR, FAILURE);
   */
  
  template <typename T, char* p[]>
  struct Enumeration {
  public:
    T value;

    void operator=(T v) {
      value = v;
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
      return p[value];
    }

    int getValue() {
      return (int)value;
    }
    
    template <class X>
    static ::std::string IMAGE(X& r) {
      return r.toString();
    }

    int LENGTH() { return 1; }
    
    bool operator ==(const Enumeration<T, p> &other) const { return value == other.value; }
    bool operator !=(const Enumeration<T, p> &other) const { return value != other.value; }
  };

  template <char p[]>
  struct CharArray {
    public:
    char value;

    std::string toString() {
      return std::string(1, value);
    }

    void operator=(char c) { value = c; }
    bool operator ==(const CharArray<p> &other) const { return value == other.value; }
    bool operator !=(const CharArray<p> &other) const { return value != other.value; }
    bool operator ==(const char other) const { return value == other; }
    bool operator !=(const char other) const { return value != other; }
    
    int LENGTH() { return 1; }
    
    int getValue() {
      return value - '0';
    }

    template <class T>
    static ::std::string IMAGE(T& r) {
      return r.toString();
    }

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
    TYPE operator ()(int index) {
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
