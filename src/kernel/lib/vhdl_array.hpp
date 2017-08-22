#ifndef VHDL_ARRAY_HPP
#define VHDL_ARRAY_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>

namespace vhdl {

  /*
   * Array example:
   *        <NAME>                <RANGE>          <TYPE>
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class RANGE, class TYPE>
  class Array {
  public:
    struct Range {
      RANGE left;
      RANGE right;
    };
    Range range;
    TYPE* value = NULL;

    void init(RANGE left, RANGE right) {
      range.left = left;
      range.right = right;
    }
    
    void init(const Array<RANGE, TYPE>& other) {
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
    
    Array() {
    }

    Array(RANGE left, RANGE right) {
      range.left = left;
      range.right = right;
    }
    
    explicit Array(int size) {
      resize(size);
      init();
    }
    
    void resize(int size) {
      assert(size > 0);
      range.right = ASCENDING() ? range.left + size - 1 : range.left - size + 1;
    }
    
    void set(Array<RANGE, TYPE>& other) {
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

    TYPE& operator[](RANGE index) {
      return get(index.POS());
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
