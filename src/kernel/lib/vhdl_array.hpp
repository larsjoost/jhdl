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

namespace vhdl {

  /*
   * Array example:
   *        <NAME>                <RANGE>          <SUBTYPE>
   * type BIT_VECTOR is array (NATURAL range <>) of BIT;
   */
  
  template<class RANGE, class SUBTYPE>
  class Array {

    Exceptions m_exceptions;
    
    inline void setAll(SUBTYPE& value) {
      for (int i = 0; i < LENGTH() - 1; i++) {
        m_content[i] = value;
      }
    }

  protected:
    
    std::string m_name;
    
    RANGE m_range;
    bool m_constrained = false;
    std::vector<SUBTYPE> m_content;
    SUBTYPE m_subtype;

  public:

    Array(const char* name) : m_name(name) {};
    Array() {};

    void resize(int length) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("resize(length = " + std::to_string(length) + ")");
      m_content = std::vector<SUBTYPE>(length);
      debug.debug("m_content.size() = " + std::to_string(m_content.size()));
      for (int i = 0; i < length; i++) {
	debug.debug("allocate m_content[" + std::to_string(i) + ")");
	m_content.push_back(SUBTYPE());
      }
      debug.debug("m_content.size() = " + std::to_string(m_content.size()));
      debug.functionEnd("resize");
    }

    void constrain() {
      // TODO: Implement
    }
    
    void constrain(const Array<RANGE, SUBTYPE>& other) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("constrain(other = " + other.info() + ")");
      m_range.constrain(other.m_range);
      m_subtype.constrain(other.m_subtype);
      resize(LENGTH());
      debug.functionEnd("construct");
    }

    void constrain(int left, int right, bool ascending = true) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("constrain(left = " + std::to_string(left) + ", right = " + std::to_string(right) +
			  ", ascending = " + std::to_string(ascending) + ")");
      m_range.constrain(left, right, ascending);
      int resize_length = LENGTH();
      debug.debug("Resize length = " + std::to_string(resize_length));
      resize(resize_length);
      debug.functionEnd("construct");
    }

    void setString(const std::string& s) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("setString(s = " + s + ")");
      if (s.size() != LENGTH()) {
	throw Exceptions::RuntimeError("Length of left size " + std::to_string(s.size()) + " does not match length of right side = " + std::to_string(LENGTH()),
				       m_name);
      }
      debug.debug("m_content size = " + std::to_string(m_content.size()));
      int i = 0;
      for (auto c : s) {
	SUBTYPE& a = m_content.at(i);
	debug.debug("Before: a.info[" + std::to_string(i) + "] = " + a.info());
	a = c;
	debug.debug("After: a.info[" + std::to_string(i) + "] = " + a.info());
	debug.debug("m_content[" + std::to_string(i) + "] = " + a.toString());
	i++;
      }
      debug.functionEnd("setString: " + toString());
    }

    void set(Array<RANGE, SUBTYPE>& other) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("set(other = " + other.toString() + ")");
      if (LENGTH() != other.LENGTH()) {
        constrain(1, other.LENGTH());
      }
      for (int i = 0; i < LENGTH(); i++) {
        // std::cout << "set[" << i << "] = " << other.m_content[i].IMAGE(other.m_content[i]) << std::endl;
	m_content[i] = other.m_content[i];
      }
      debug.functionEnd("set");
    }

    int ConvertIndex(int index) {
      Debug<true> debug = Debug<true>(this);
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
      Debug<true> debug = Debug<true>(this);
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
      SUBTYPE& result = m_content.at(i);
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
      //      a_debug.functionStart("init");
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
        m_content[i] = vec[i];
      }
      //      a_debug.functionEnd("init");
    }

    void operator=(const SUBTYPE other) {
      Debug<true> debug = Debug<true>("Array::operator=(const SUBTYPE other)");
      debug.functionStart("operator=(other = " + other.toString() + ")");
      m_content = other;
      debug.functionEnd("operator=");
    }
    void operator=(const char* other) {
      Debug<true> debug = Debug<true>("Array::operator=(const char* other)");
      debug.functionStart("operator=(other = " + std::string(other) + ")");
      std::string s(other);
      setString(s);
      debug.functionEnd("operator=");
    }
    void operator=(const std::string other) {
      Debug<true> debug = Debug<true>("Array::operator=(const std::string other)");
      debug.functionStart("operator=(other = " + other + ")");
      setString(other);
      debug.functionEnd("operator=");
    }
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
      a_debug.functionEnd("operator[]: " + result.info());
      return result;
    }
    
    inline bool ASCENDING() {
      return m_range.ASCENDING();
    }
    
    inline unsigned int LENGTH() {
      Debug<true> debug = Debug<true>(this);
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
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("toString");
      std::string s;
      for (int i = m_range.LEFT(); WithinRange(i); i += (m_range.ASCENDING() ? 1 : -1)) {
        s += this->get(i).toString(false);
      }
      debug.functionStart("toString:" +s);
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
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("setOthers(value = " + value.toString() + ")");
      setAll(value);
      debug.functionEnd("setOthers");
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setIndex(int index, SUBTYPE value) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("setIndex(index = " + std::to_string(index) + ")");
      m_content[index] = value;
      debug.functionEnd("setIndex");
      return *this;
    }

    inline Array<RANGE, SUBTYPE>& setElement(RANGE element, SUBTYPE value) {
      Debug<true> debug = Debug<true>(this);
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
