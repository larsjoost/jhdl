#ifndef VHDL_TYPE_HPP
#define VHDL_TYPE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>

namespace vhdl {
  
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
    
    int LENGTH() { return 1; }
    
    int getValue() {
      return value - '0';
    }

    template <class T>
    static ::std::string IMAGE(T& r) {
      return r.toString();
    }

  };

  
}

#endif
