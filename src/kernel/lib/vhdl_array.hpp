#ifndef VHDL_ARRAY_HPP
#define VHDL_ARRAY_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include <vector>

// #include "../../debug/debug.hpp"

namespace vhdl {

  /*
   * Array example:
   *        <NAME>                <RANGE>          <TYPE>
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class RANGE, class TYPE>
  class Array {
    //    Debug<true> a_debug;
  public:
    RANGE a_left;
    RANGE a_right;
    std::vector<TYPE> a_value;

    void construct(RANGE left, RANGE right) {
      a_left = left;
      a_right = right;
    }
    
    void construct(const Array<RANGE, TYPE>& other) {
      a_left = other.a_left;
      a_right = other.a_right;
    }

    void set(const std::string& s) {
      //      a_debug.functionStart("set");
      assert(s.size() == LENGTH());
      a_value.clear();
      for (auto c : s) {
        TYPE t;
        t = c;
        a_value.push_back(t);
      }
      //      a_debug.functionEnd("set");
    }

    int ConvertIndex(int index) {
      int x = ASCENDING() ? 1 : -1;
      return x * index - a_left;
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
      return a_value[i];
    }
    
    Array() { // : a_debug("Array") {
    }

    Array(RANGE left, RANGE right) { // : a_debug("Array") {
      a_left = left;
      a_right = right;
    }

    Array(std::vector<int> vec) { // : a_debug("Array") {
      assert(LENGTH() == vec.size());
    }
    
    explicit Array(int size) { // : a_debug("Array") {
      resize(size);
    }

    void init(std::vector<TYPE> vec) {
      //      a_debug.functionStart("init");
      assert(LENGTH() == vec.size());
      if (a_value.size() != vec.size()) { a_value.resize(vec.size()); }
      for (int i = 0; i < vec.size(); i++) {
        a_value[i] = vec[i];
      }
      //      a_debug.functionEnd("init");
    }

    void resize(int size) {
      assert(size > 0);
      a_right = ASCENDING() ? a_left.ToInt() + size - 1 : a_left.ToInt() - size + 1;
    }
    
    void set(Array<RANGE, TYPE>& other) {
      //      a_debug.functionStart("set(other = " + other.toString() + ")");
      a_left = other.a_left;
      a_right = other.a_right;
      init(other.a_value);
      //      a_debug.functionEnd("set");
    }

    void operator=(const TYPE other) { a_value = other; }
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
      return (a_right > a_left ? true : false);
    }
    
    unsigned int LENGTH() { return abs(a_left - a_right) + 1; }
    TYPE& HIGH() { return (a_left > a_right ? LEFT() : RIGHT()); }
    TYPE& LOW() { return (a_left < a_right ? LEFT() : RIGHT()); }
    TYPE& LEFT() { return a_value[0]; }
    TYPE& RIGHT() { return a_value[LENGTH() - 1]; }

    std::string toString() {
      std::string s;
      for (int i = a_left; WithinRange(i); ASCENDING() ? i++ : i--) {
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
