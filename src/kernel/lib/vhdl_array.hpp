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

    inline void setAll(SUBTYPE& value) {
      for (int i = 0; i < LENGTH() - 1; i++) {
        m_content[i] = value;
      }
    }

  protected:

    RANGE m_range;
    std::vector<SUBTYPE> m_content;
    SUBTYPE m_subtype;

  public:

    Array() {};
    Array(RANGE& range, SUBTYPE& subtype) : m_range(range), m_subtype(subtype) {};
    
    void construct(const Array<RANGE, SUBTYPE>& other) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("construct(other = " + other.info() + ")");
      m_range.construct(other.m_range);
      m_subtype.construct(other.m_subtype);
      m_content.resize(LENGTH());
      debug.functionEnd("construct");
    }

    void construct(int left, int right, bool ascending = true) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("construct(left = " + std::to_string(left) + ", right = " + std::to_string(right) +
			  ", ascending = " + std::to_string(ascending) + ")");
      m_range.construct(left, right, ascending);
      m_content.resize(LENGTH());
      debug.functionEnd("construct");
    }

    void set(const std::string& s) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("set");
      if (s.size() != LENGTH()) {
        construct(1, s.size());
      }
      int i = 0;
      for (auto c : s) {
        m_content[i++] = c;
      }
      debug.functionEnd("set");
    }

    void set(Array<RANGE, SUBTYPE>& other) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("set(other = " + other.toString() + ")");
      if (LENGTH() != other.LENGTH()) {
        construct(1, other.LENGTH());
      }
      for (int i = 0; i < LENGTH(); i++) {
        // std::cout << "set[" << i << "] = " << other.m_content[i].IMAGE(other.m_content[i]) << std::endl;
        m_content[i] = other.m_content[i];
      }
      debug.functionEnd("set");
    }

    int ConvertIndex(int index) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("ConvertIndex(index = " + std::to_string(index) + ")");
      debug.debug("m_range: " + m_range.info());
      int x = ASCENDING() ? 1 : -1;
      RANGE left = m_range.LEFT();
      debug.debug("left: " + left.info());
      int left_pos = m_range.POS(left);
      debug.debug("x = " + std::to_string(x) + ", left_pos = " + std::to_string(left_pos));
      int result = x * index - left_pos;
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
      assert(i >= 0 && i < m_content.size());
      SUBTYPE& result = m_content[i];
      debug.functionEnd("get");
      return result;
    }
    
    bool equals(const std::string& other) {
      assert(LENGTH() == other.size());
      for (int i = 0; i < LENGTH(); i++) {
        if (m_content[i] != other[i]) {
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
        m_content[i] = vec[i];
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
        m_content[i] = vec[i];
      }
      //      a_debug.functionEnd("init");
    }

    void operator=(const SUBTYPE other) { m_content = other; }
    void operator=(const char* other) {std::string s(other); set(s);}
    void operator=(const std::string other) {set(other);}
    bool operator!=(const Array<RANGE, SUBTYPE>& other) {
      assert(this->LENGTH() != other.LENGTH());
      for (int i = 0; i < m_content.size(); i++) {
        if (m_content[i] != other.m_content[i]) {
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
      return m_range.ASCENDING();
    }
    
    inline unsigned int LENGTH() {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("LENGTH");
      int result = m_range.LENGTH();
      debug.functionEnd("LENGTH: " + std::to_string(result));
      return result;
    }
    
    inline SUBTYPE& HIGH() { return VAL(m_range.HIGH()); };
    inline SUBTYPE& LOW() { return VAL(m_range.LOW()); }
    inline SUBTYPE& ELEMENT() { return LOW(); }
    inline SUBTYPE& LEFT() { return VAL(m_range.LEFT()); }
    inline SUBTYPE& RIGHT() { return VAL(m_range.RIGHT()); }
    inline SUBTYPE& VAL(RANGE index) { return get(index.POS()); }
    inline RANGE& POS(RANGE value) { return value.POS(); }  
    
    std::string toString() {
      std::string s;
      for (int i = m_range.LEFT(); WithinRange(i); i += (m_range.ASCENDING() ? 1 : -1)) {
        s += this->get(i).toString(false);
      }
      return s;
    }

    std::string info() const {
      return "Array: m_range = (" + m_range.info() + "), m_subtype = (" + m_subtype.info() + ")";
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
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("setOthers(value = " + value.toString() + ")");
      setAll(value);
      debug.functionEnd("setOthers");
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setIndex(int index, SUBTYPE value) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("setIndex(index = " + std::to_string(index) + ")");
      m_content[index] = value;
      debug.functionEnd("setIndex");
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setElement(RANGE element, SUBTYPE value) {
      Debug<true> debug = Debug<true>("Array");
      debug.functionStart("setElement(element = " + element.toString() + ")");
      debug.debug("1");
      int i = element.POS(element);
      debug.debug("2");
      m_content[i] = value;
      debug.functionEnd("setElement");
      return *this;
    }
  
  };

}

#endif
