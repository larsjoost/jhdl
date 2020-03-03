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

#include "../../debug/debug.hpp"

#include "vhdl_enum.hpp"

namespace vhdl {

  /*
   * Array example:
   *        <NAME>                <RANGE>          <SUBTYPE>
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class RANGE, class SUBTYPE>
  class Array {
    bool m_constructed = false;
    bool m_ascending = true;
    
    inline void setAll(SUBTYPE& value) {
      for (int i = 0; i < LENGTH() - 1; i++) {
        a_value[i] = value;
      }
    }

  public:
    RANGE a_left;
    RANGE a_right;
    std::vector<SUBTYPE> a_value;
    SUBTYPE a_subtype;
    
    void construct(RANGE left, RANGE right, bool ascending = true) {
      m_constructed = true;
      m_ascending = ascending;
      a_left = left;
      a_right = right;
      a_value.resize(LENGTH());
      setAll(a_subtype);
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
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("ConvertIndex(index = " + std::to_string(index) + ")");
      int x = ASCENDING() ? 1 : -1;
      int left_pos = POS(a_left);
      debug.debug("x = " + std::to_string(x) + ", a_left = " + std::to_string(left_pos));
      int result = x * index - a_left;
      debug.functionEnd("ConvertIndex: " + std::to_string(result) + ")");
      return result;
    }

    inline bool IndexCheck(int index) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("IndexCheck(index = " + std::to_string(index) + ")");
      bool result = (index >= 0 && index < LENGTH());
      debug.functionEnd("IndexCheck: " + std::string(result ? "true" : "false"));
      return result;
    }
    
    bool WithinRange(int index) {
      int i = ConvertIndex(index);
      return IndexCheck(i);
    }
    
    inline SUBTYPE& get(int index) {
      Debug<true> debug = Debug<true>("Array::get");
      debug.functionStart("get(index = " + std::to_string(index) + ")");
      int i = ConvertIndex(index);
      debug.debug("i = " + std::to_string(i));
      assert(IndexCheck(i));
      assert(i >= 0 && i < a_value.size());
      SUBTYPE& result = a_value[i];
      debug.functionEnd("get");
      return result;
    }
    
    Array() {
    }

    bool equals(const std::string& other) {
      assert(LENGTH() == other.size());
      for (int i = 0; i < LENGTH(); i++) {
        if (a_value[i] != other[i]) {
          return false;
        }
      }
      return true;
    }

    Array(RANGE left, RANGE right, bool ascending = true) {
      construct(left, right, ascending);
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
    
    inline SUBTYPE& operator[](int index) {
      Debug<true> debug = Debug<true>("Array::operator[](int index)");
      debug.functionStart("operator[" + std::to_string(index) + "]");
      debug.functionEnd("operator[]");
      return get(index);
    }

    inline SUBTYPE& operator[](RANGE index) {
      Debug<true> a_debug = Debug<true>("Array::operator[](RANGE index)");
      a_debug.functionStart("operator[" + index.toString() + "]");
      int i = index.POS();
      a_debug.debug("i = " + std::to_string(i));
      SUBTYPE& result = get(i); 
      a_debug.functionEnd("operator[]");
      return result;
    }
    
    inline bool ASCENDING() {
      return m_ascending;
    }
    
    inline unsigned int LENGTH() {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("LENGTH");
      int result = abs(POS(a_left) - POS(a_right)) + 1;
      debug.functionEnd("LENGTH: " + std::to_string(result));
      return result;
    }
    
    inline SUBTYPE& HIGH() { return (a_left > a_right ? LEFT() : RIGHT()); }
    inline SUBTYPE& LOW() { return (a_left < a_right ? LEFT() : RIGHT()); }
    inline SUBTYPE& ELEMENT() { return a_value[0]; }
    inline SUBTYPE& LEFT() { return a_value[0]; }
    inline SUBTYPE& RIGHT() { return a_value[LENGTH() - 1]; }

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

    inline Array<RANGE, SUBTYPE>& setOthers(SUBTYPE value) {
      setAll(value);
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setIndex(int index, SUBTYPE value) {
      a_value[index] = value;
      return *this;
    }

    inline int POS(int index) {
      return index;
    }

    inline int POS(auto& index) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("POS[" + index.toString() + "]");
      int i =  index.POS();
      debug.functionEnd("POS: " + std::to_string(i));
      return i;
    }
    
    inline Array<RANGE, SUBTYPE>& setElement(RANGE element, SUBTYPE value) {
      int i = POS(element);
      a_value[i] = value;
      return *this;
    }
  
  };

}

#endif
