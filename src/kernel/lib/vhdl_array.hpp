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
#include "../../exceptions/exceptions.hpp"

#include "vhdl_enum.hpp"
#include "vhdl_exception.hpp"

namespace vhdl {

  /*
   * Array example:
   *        <NAME>                <RANGE>          <SUBTYPE>
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class RANGE, class SUBTYPE>
  class Array {

    Debug<true> m_debug;
    Exceptions m_exceptions;
    
    inline void setAll(SUBTYPE& value) {
      for (int i = 0; i < LENGTH() - 1; i++) {
        m_content.at(i) = value;
      }
    }

  protected:
    
    std::string m_name;
    
    RANGE m_range;
    bool m_constrained = false;
    std::vector<SUBTYPE> m_content;
    SUBTYPE m_subtype;

  public:

    Array(const char* name) : m_debug(this), m_name(name), m_subtype(name) {
      m_debug.functionStart("Array(name = " + std::string(name) + ")");
      m_debug.functionEnd("Array");
    };
    Array() : m_debug(this) {
      m_debug.functionStart("Array");
      m_debug.functionEnd("Array");
    };

    void resize(int length) {
      m_debug.functionStart("resize(length = " + std::to_string(length) + ")");
      m_debug.debug("LENGTH() = " + std::to_string(LENGTH()));
      m_content = std::vector<SUBTYPE>();
      for (int i = 0; i < length; i++) {
	m_content.push_back(SUBTYPE());
      }
      m_debug.debug("m_content.size() = " + std::to_string(m_content.size()));
      for (int i = 0; i < length; i++) {
	SUBTYPE& a = m_content.at(i);
	a.constrain(m_subtype);
	m_debug.debug("Constraining a[" + std::to_string(i) + "] = " + a.info());
      }
      m_debug.debug("m_content.size() = " + std::to_string(m_content.size()));
      m_debug.functionEnd("resize");
    }

    void constrain() {
      m_range.constrain();
      m_subtype.constrain();
    }
    
    void constrain(const Array<RANGE, SUBTYPE>& other) {
      m_debug.functionStart("constrain");
      m_range.constrain(other.m_range);
      m_subtype.constrain(other.m_subtype);
      resize(LENGTH());
      m_debug.functionEnd("construct");
    }

    void constrain(int left, int right, bool ascending = true) {
      m_debug.functionStart("constrain(left = " + std::to_string(left) + ", right = " + std::to_string(right) +
			  ", ascending = " + std::to_string(ascending) + ")");
      m_debug.debug("m_name = " + m_name);
      m_range.constrain(left, right, ascending);
      m_subtype.constrain();
      int resize_length = LENGTH();
      m_debug.debug("Resize length = " + std::to_string(resize_length));
      resize(resize_length);
      m_debug.functionEnd("construct");
    }

    void constrain(SUBTYPE& subtype) {
      m_debug.functionStart("constrain(subtype = " + subtype.toString() + ")");
      m_range.constrain();
      m_subtype.constrain(subtype);
      m_debug.functionEnd("construct");
    }

    void constrain(int left, int right, bool ascending, SUBTYPE& subtype) {
      m_debug.functionStart("constrain(left = " + std::to_string(left) + ", right = " + std::to_string(right) +
			  ", ascending = " + std::to_string(ascending) + ", subtype = " + subtype.toString() + ")");
      m_range.constrain(left, right, ascending);
      m_subtype.constrain();
      m_debug.functionEnd("construct");
    }

    Array<RANGE, SUBTYPE> clone() {
      Array<RANGE, SUBTYPE> x;
      x.constrain(*this);
      return x;
    }
    
    void setString(const std::string& s) {
      m_debug.functionStart("setString(s = " + s + ")");
      if (LENGTH() != s.size()) {
	m_exceptions.printError("Length of left size " + std::to_string(LENGTH()) + " ( " + m_name +
				 ") does not match length of right side = " + std::to_string(s.size()) + " (\"" + s + "\")",
				 __FILE__, __LINE__);
	throw RuntimeError();
      }
      m_debug.debug("m_content size = " + std::to_string(m_content.size()));
      int i = 0;
      for (auto c : s) {
	SUBTYPE& a = m_content.at(i);
	m_debug.debug("Before: a.info[" + std::to_string(i) + "] = " + a.info());
	a = c;
	m_debug.debug("After: a.info[" + std::to_string(i) + "] = " + a.info());
	m_debug.debug("m_content[" + std::to_string(i) + "] = " + a.toString());
	i++;
      }
      m_debug.functionEnd("setString: " + toString());
    }

    void set(Array<RANGE, SUBTYPE>& other) {
      m_debug.functionStart("set(other = " + other.info() + ")", false, __FILE__, __LINE__);
      if (LENGTH() != other.LENGTH()) {
	m_debug.debug("LENGTH() = " + std::to_string(LENGTH()) + " != " + std::to_string(other.LENGTH()));
        constrain(1, other.LENGTH());
      }
      for (int i = 0; i < LENGTH(); i++) {
        // std::cout << "set[" << i << "] = " << other.m_content[i].IMAGE(other.m_content[i]) << std::endl;
	m_content.at(i) = other.m_content.at(i);
      }
      m_debug.functionEnd("set");
    }

    int ConvertIndex(int index) {
      m_debug.functionStart("ConvertIndex(index = " + std::to_string(index) + ")");
      m_debug.debug("m_range: " + m_range.info());
      int x = ASCENDING() ? 1 : -1;
      RANGE left = m_range.LEFT();
      m_debug.debug("left: " + left.info());
      int left_pos = m_range.POS(left);
      m_debug.debug("x = " + std::to_string(x) + ", left_pos = " + std::to_string(left_pos));
      int result = x * index - left_pos;
      m_debug.functionEnd("ConvertIndex: " + std::to_string(result) + ")");
      return result;
    }

    inline bool IndexCheck(int index) {
      m_debug.functionStart("IndexCheck(index = " + std::to_string(index) + ")");
      bool result = (index >= 0 && index < LENGTH());
      m_debug.functionEnd("IndexCheck: " + std::string(result ? "true" : "false"));
      return result;
    }
    
    bool WithinRange(int index) {
      int i = ConvertIndex(index);
      return IndexCheck(i);
    }
    
    inline SUBTYPE& get(int index) {
      m_debug.functionStart("get(index = " + std::to_string(index) + ")");
      int i = ConvertIndex(index);
      m_debug.debug("i = " + std::to_string(i));
      assert(IndexCheck(i));
      assert(i >= 0 && i < m_content.size());
      SUBTYPE& result = m_content.at(i);
      m_debug.functionEnd("get");
      return result;
    }
    
    bool equals(const std::string& other) {
      assert(LENGTH() == other.size());
      for (int i = 0; i < LENGTH(); i++) {
        if (m_content.at(i) != other[i]) {
          return false;
        }
      }
      return true;
    }

    Array(RANGE left, RANGE right, bool ascending = true) {
      constrain(left, right, ascending);
    }
    /*
    Array(std::vector<int> vec) { // : a_debug(this) {
      if (LENGTH() != vec.size()) {
        std::cout << "Warning: Array of length " << LENGTH() << " assigned to integer array of length " << vec.size() << std::endl;
        int l = 0;
        int r = vec.size() - 1;
        RANGE left;
        RANGE right;
        left = l;
        right = r;
        constrain(left, right);
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
      constrain(0, s.size() - 1);
      set(s);
    }

    void init(std::vector<SUBTYPE> vec) {
      m_debug.functionStart("init");
      if (LENGTH() != vec.size()) {
        std::cout << "Warning: Array of length " << LENGTH() << " assigned to array of length " << vec.size() << std::endl;
        int l = 0;
        int r = vec.size() - 1;
        RANGE left;
        RANGE right;
        left = l;
        right = r;
        constrain(left, right);
      }
      for (int i = 0; i < LENGTH(); i++) {
	// std::cout << "init: vec[" << i << "] = " << vec[i].toString() << std::endl;
        m_content.at(i) = vec.at(i);
      }
      m_debug.functionEnd("init");
    }

    friend const std::string operator+(const char* s, Array<RANGE, SUBTYPE>& r) {
      std::string s1(s);
      std::string s2 = r.toString();
      std::string result = s1 + s2;
      return result;
    }
    
    void operator=(const SUBTYPE other) {
      m_debug.functionStart("operator=(other = " + other.toString() + ")", false, __FILE__, __LINE__);
      m_content = other;
      m_debug.functionEnd("operator=");
    }
    void operator=(const char* other) {
      m_debug.functionStart("operator=(other = " + std::string(other) + ")", false, __FILE__, __LINE__);
      std::string s(other);
      setString(s);
      m_debug.functionEnd("operator=");
    }
    void operator=(const std::string other) {
      m_debug.functionStart("operator=(other = " + other + ")", false, __FILE__, __LINE__);
      setString(other);
      m_debug.functionEnd("operator=");
    }
    void operator=(Array<RANGE, SUBTYPE>& other) {
      m_debug.functionStart("operator=(other = " + other.info() + ")", false, __FILE__, __LINE__);
      set(other);
      m_debug.functionEnd("operator=");
    }
    bool operator!=(const Array<RANGE, SUBTYPE>& other) {
      assert(this->LENGTH() != other.LENGTH());
      for (int i = 0; i < m_content.size(); i++) {
        if (m_content.at(i) != other.m_content.at(i)) {
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
      m_debug.functionStart("operator[" + std::to_string(index) + "]", false, __FILE__, __LINE__);
      m_debug.functionEnd("operator[]");
      return get(index);
    }

    inline SUBTYPE& operator[](RANGE index) {
      m_debug.functionStart("operator[" + index.toString() + "]", false, __FILE__, __LINE__);
      int i = index.POS();
      m_debug.debug("i = " + std::to_string(i));
      SUBTYPE& result = get(i); 
      m_debug.functionEnd("operator[]: " + result.info());
      return result;
    }
    
    inline bool ASCENDING() {
      return m_range.ASCENDING();
    }
    
    inline unsigned int LENGTH() {
      m_debug.functionStart("LENGTH");
      int result = m_range.LENGTH();
      m_debug.functionEnd("LENGTH: " + std::to_string(result));
      return result;
    }
    
    inline SUBTYPE& HIGH() { return VAL(m_range.HIGH()); };
    inline SUBTYPE& LOW() { return VAL(m_range.LOW()); }
    inline SUBTYPE& ELEMENT() {
      m_debug.functionStart("ELEMENT", false, __FILE__, __LINE__);
      SUBTYPE& x = LOW(); 
      m_debug.functionEnd("ELEMENT: " + x.info());
      return x; }
    inline SUBTYPE& LEFT() { return VAL(m_range.LEFT()); }
    inline SUBTYPE& RIGHT() { return VAL(m_range.RIGHT()); }
    inline SUBTYPE& VAL(RANGE index) { return get(index.POS()); }
    inline RANGE& POS(RANGE value) { return value.POS(); }  
    
    std::string toString() {
      m_debug.functionStart("toString", false, __FILE__, __LINE__);
      std::string s;
      for (int i = m_range.LEFT(); WithinRange(i); i += (m_range.ASCENDING() ? 1 : -1)) {
        s += get(i).toString();
      }
      m_debug.functionStart("toString:" +s);
      return s;
    }

    std::string info() {
      std::string s = "Array: m_range = (" + m_range.info() + "),\nm_subtype = (" + m_subtype.info() + "[" + std::to_string(m_content.size()) + "]) = {";
      std::string delimiter;
      int size = 0;
      for (auto& i : m_content) {
	s += delimiter + i.info();
	delimiter = ", ";
	size++;
      }
      s += "} [" + std::to_string(size) + "]\n";
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
      m_debug.functionStart("setOthers(value = " + value.toString() + ")", false, __FILE__, __LINE__);
      setAll(value);
      m_debug.functionEnd("setOthers");
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setIndex(int index, SUBTYPE value) {
      m_debug.functionStart("setIndex(index = " + std::to_string(index) + ", value = " + value.info() + ")", false, __FILE__, __LINE__);
      m_debug.debug("info = " + info());
      m_content.at(index) = value;
      m_debug.debug("info = " + info());
      m_debug.functionEnd("setIndex");
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setElement(RANGE element, SUBTYPE value) {
      m_debug.functionStart("setElement(element = " + element.toString() + ")", false, __FILE__, __LINE__);
      int i = element.POS(element);
      m_content.at(i) = value;
      m_debug.functionEnd("setElement");
      return *this;
    }
  
    inline Array<RANGE, SUBTYPE>& setArray(SUBTYPE& value) {
      m_debug.functionStart("setArray(value = " + value.info() + ")", false, __FILE__, __LINE__);
      m_debug.functionEnd("setArray");
      return *this;
    }

  };

}

#endif
