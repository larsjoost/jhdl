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
   *        <NAME>                <RANGE>          <SUBTYPE>
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class RANGE, class SUBTYPE>
  class Array {
    //    Debug<true> a_debug;
  public:
    RANGE a_left;
    RANGE a_right;
    std::vector<SUBTYPE> a_value;
    SUBTYPE a_subtype;

    void construct(RANGE left, RANGE right) {
      a_left = left;
      a_right = right;
      a_value.resize(LENGTH());
      for (int i = 0; i < LENGTH() - 1; i++) {
        a_value[i] = a_subtype;
      }
    }

    void construct(const Array<RANGE, SUBTYPE>& other) {
      construct(other.a_left, other.a_right);
    }

    void SubtypeConstruct(const SUBTYPE& other) {
      a_subtype.construct(other);
    }
    
    void SubtypeInit(const SUBTYPE& other) {
      a_subtype.init(other);
    }

    void set(const std::string& s) {
      //      a_debug.functionStart("set");
      // std::cout << "a_left = " << a_left.IMAGE(a_left) << ", a_right = " << a_right.IMAGE(a_right) << std::endl;
      if (s.size() != LENGTH()) {
        construct(1, s.size());
      }
      int i = 0;
      for (auto c : s) {
        a_subtype = c;
        a_value[i++] = a_subtype;
      }
      //      a_debug.functionEnd("set");
    }

    void set(Array<RANGE, SUBTYPE>& other) {
      //      a_debug.functionStart("set(other = " + other.toString() + ")");
      if (LENGTH() != other.LENGTH()) {
        construct(1, other.LENGTH());
      }
      for (int i = 0; i < LENGTH(); i++) {
        // std::cout << "set[" << i << "] = " << other.a_value[i].IMAGE(other.a_value[i]) << std::endl;
        a_value[i] = other.a_value[i];
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
    
    SUBTYPE& get(int index) {
      int i = ConvertIndex(index);
      assert(IndexCheck(i));
      assert(i >= 0 && i < a_value.size());
      return a_value[i];
    }
    
    Array() { // : a_debug("Array") {
    }

    Array(RANGE left, RANGE right) { // : a_debug("Array") {
      construct(left, right);
    }
    /*
    Array(std::vector<int> vec) { // : a_debug("Array") {
      if (LENGTH() != vec.size()) {
        std::cout << "Warning: Array of length " << LENGTH() << " assigned to integer array of length " << vec.size() << std::endl;
        int l = 0;
        int r = vec.size() - 1;
        RANGE left;
        RANGE right;
        left = l;
        right = r;
        construct(left, right);
      }
      for (int i = 0; i < LENGTH(); i++) {
        // std::cout << "Array: vec[" << i << "] = " << vec[i] << std::endl;
        a_value[i] = vec[i];
      }
    }
    */
    Array(std::vector<SUBTYPE> vec) {
      init(vec);
    }

    void init_string(const std::string& s) {
      construct(0, s.size() - 1);
      set(s);
    }
    void init(std::vector<SUBTYPE> vec) {
      //      a_debug.functionStart("init");
      if (LENGTH() != vec.size()) {
        std::cout << "Warning: Array of length " << LENGTH() << " assigned to array of length " << vec.size() << std::endl;
        int l = 0;
        int r = vec.size() - 1;
        RANGE left;
        RANGE right;
        left = l;
        right = r;
        construct(left, right);
      }
      for (int i = 0; i < LENGTH(); i++) {
         // std::cout << "init: vec[" << i << "] = " << vec[i].toString() << std::endl;
        a_value[i] = vec[i];
      }
      //      a_debug.functionEnd("init");
    }

    void operator=(const SUBTYPE other) { a_value = other; }
    void operator=(const char* other) {std::string s(other); set(s);}
    void operator=(const std::string other) {set(other);}
    bool operator!=(const Array<RANGE, SUBTYPE>& other) {
      assert(this->LENGTH() != other.LENGTH());
      for (int i = 0; i < a_value.size(); i++) {
        if (a_value[i] != other.a_value[i]) {
          return false;
        }
      }
      return true;
    }
    bool operator!=(const std::string other) {
      Array<RANGE, SUBTYPE> o;
      o.init_string(other);
      return (*this != o);
    }
    
    SUBTYPE& operator [](int index) {
      return get(index);
    }

    SUBTYPE& operator[](RANGE index) {
      return get(index.POS());
    }
    
    bool ASCENDING() {
      return (a_right > a_left ? true : false);
    }
    
    unsigned int LENGTH() const { return abs(a_left - a_right) + 1; }
    SUBTYPE& HIGH() { return (a_left > a_right ? LEFT() : RIGHT()); }
    SUBTYPE& LOW() { return (a_left < a_right ? LEFT() : RIGHT()); }
    SUBTYPE& LEFT() { return a_value[0]; }
    SUBTYPE& RIGHT() { return a_value[LENGTH() - 1]; }

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

    static ::std::string IMAGE(SUBTYPE r) {
      return std::to_string(r);
    }

  };

}

#endif
