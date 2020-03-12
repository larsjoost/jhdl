#ifndef VHDL_ENUM_HPP
#define VHDL_ENUM_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>

#include "../../debug/debug.hpp"

namespace vhdl {


  /*
   * Enumeration example:
   *
   * type SEVERITY_LEVEL is (NOTE, WARNING, ERROR, FAILURE);
   */

  template<typename T>
  struct EnumerationElement {
    T e;
    char c;
    const char* s;
  };

  /*
  template<typename T, int N>
  struct EnumerationElementArray {
    ElumerationElement<T> data[N];
  };
  */
  
  /*
    Example boolean:

    enum class BOOLEAN_enum {FALSE, TRUE};
  
    struct BOOLEAN_value {
    const static int size = 2;
    const static int enum_size = 2;
    vhdl::EnumerationElement<BOOLEAN_enum> array[size] {{BOOLEAN_enum::FALSE, 0, "FALSE"}, {BOOLEAN_enum::TRUE, 0, "TRUE"}};
    };

    using BOOLEAN = vhdl::Enumeration<BOOLEAN_enum, BOOLEAN_value>;
  
  */

  template <typename T, class E, int SIZE, int ENUM_SIZE, int CHAR_SIZE>
  class Enumeration {

    Debug<true> m_debug;
    std::string m_name;

    const E valueArray {};
    
    template<int N>
    struct Array {
      int table[N];
    };
  
    int underlying_index(T e) {
      return static_cast<std::underlying_type_t<T>>(e);
    }

    constexpr Array<CHAR_SIZE> char_position_lookup_initialization() {
      Array<CHAR_SIZE> lut {};
      for (int i=0; i<CHAR_SIZE; i++) {
	lut.table[i] = -1;
      }
      for (int i=0; i<SIZE; i++) {
	int index = (int)valueArray.get(i).c;
	if (index >= CHAR_SIZE) {
	  std::string message = "valueArray[" + std::to_string(i) + "].c = " + std::to_string(index) + " > " + std::to_string(CHAR_SIZE);
	  assert(false && message.c_str());
	} else if (index != 0) {
	  lut.table[index] = i;
	}
      }
      return lut;
    }
  
    constexpr Array<ENUM_SIZE> enum_position_lookup_initialization() {
      Array<ENUM_SIZE> lut {};
      for (int i=0; i<ENUM_SIZE; i++) {
	lut.table[i] = -1;
      }
      for (int i=0; i<SIZE; i++) {
	if (valueArray.get(i).c == 0) {
	  int index = underlying_index(valueArray.get(i).e);
	  if (index >= ENUM_SIZE) {
	    assert(false);
	  } else {
	    lut.table[index] = i;
	  }
	}
      }
      return lut;
    }
    
    int char_position(char c) {
      static Array<CHAR_SIZE> char_position_lookup = char_position_lookup_initialization();
      return char_position_lookup.table[(int)c];
    }

    int enum_position(T e) {
      m_debug.functionStart("enum_position", false, __FILE__, __LINE__);
      static Array<ENUM_SIZE> enum_position_lookup = enum_position_lookup_initialization();
      int index = underlying_index(e);
      m_debug.debug("index = " + std::to_string(index));
      int result = enum_position_lookup.table[index];
      m_debug.functionEnd("enum_position: " + std::to_string(result));
      return result;
    }

    EnumerationElement<T> position(int i) {
      return valueArray.get(i);
    }

    int position(const EnumerationElement<T>& e) {
      int index;
      if (e.c == 0) {
        index = enum_position(e.e);
      } else {
        index = char_position(e.c);
      }
      return index;
    }
    
    int size() {
      return SIZE;
    }
    
  protected:

    void set(char c) {
      m_debug.functionStart("set(char c = " + std::string(1, c) + ")", false, __FILE__, __LINE__);
      if (char_position(c) < 0) {
        std::cerr << "Assigned char value " << c << " not allowed" << std::endl;
      } else {
        m_value = char_position(c);
      }
      m_debug.functionEnd("set");
    }
    
    void set(T e) {
      m_debug.functionStart("set", false, __FILE__, __LINE__);
      m_value = enum_position(e);
      m_debug.functionEnd("set: m_value = " + std::to_string(m_value));
    }
    
  public:
    int m_value = 0;
    int m_left = 0;
    int m_right = 0;

    Enumeration(const char* name) : m_debug(this), m_name(name) {
      m_debug.functionStart("Enumeration(char c = " + std::string(name) + ")", false, __FILE__, __LINE__);
      m_debug.functionEnd("Enumeration");
    };
 
    Enumeration() : m_debug(this) {
      m_debug.functionStart("Enumeration", false, __FILE__, __LINE__);
      m_left = 0;
      m_right = size() - 1;
      m_debug.functionEnd("Enumeration");
    }
    
    Enumeration(T v) : m_debug(this) {
      m_debug.functionStart("Enumeration(v = " + toString(v) + ")", false, __FILE__, __LINE__);
      set(v);
      m_left = 0;
      m_right = size() - 1;
      m_debug.functionEnd("Enumeration");
    }

    Enumeration(Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& left, Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& right)  : m_debug(this) {
      m_debug.functionStart("Enumeration", false, __FILE__, __LINE__);
      m_left = left.m_value;
      m_right = right.m_value;
      m_debug.functionEnd("Enumeration");
    }

    Enumeration(char c) : m_debug(this) {
      m_debug.functionStart("Enumeration(char c = " + std::string(1, c) + ")", false, __FILE__, __LINE__);
      set(c);
      m_debug.functionEnd("Enumeration");
    }
    
    void constrain(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& other) {
      m_left = other.m_left;
      m_right = other.m_right;
    };

    void constrain() {
      m_left = 0;
      m_right = SIZE - 1;
    };

    void operator=(T v) {
      m_debug.functionStart("operator=", false, __FILE__, __LINE__);
      set(v);
      m_debug.functionEnd("operator=");
    }

    void operator=(int v) {
      m_value = v;
    }
    
    void operator=(char c) {
      m_debug.functionStart("operator=(char c = " + std::string(1, c) + ")");
      set(c);
      m_debug.functionEnd("operator=");
    }

    void operator=(bool v) {
      m_value = v ? (T)1 : (T)0;
    }

    inline void operator=(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) { m_value = other.m_value; }
    
    bool operator!() {
      return m_value != 0;
    }
      
    operator bool() const {
      return m_value != 0;
    }

    std::string toString(int index, bool with_quotes = true) {
      m_debug.functionStart("toString(index = " + std::to_string(index) + ")", false, __FILE__, __LINE__);
      std::string result;
      if (valueArray.get(index).c == 0) {
        result = valueArray.get(index).s;
      } else {
	std::string s = with_quotes ? "'" : "";
	result = s + std::string(1, valueArray.get(index).c) + s;
      }
      m_debug.functionEnd("toString: " + result);
      return result;
    }

    std::string toString(T e, bool with_quotes = true) {
      m_debug.functionStart("toString", false, __FILE__, __LINE__);
      int index = enum_position(e);
      m_debug.debug("index = " + std::to_string(index));
      std::string result = toString(index, with_quotes);
      m_debug.functionEnd("toString: " + result);
      return result;
    }

    std::string toString(bool with_quotes = true) {
      m_debug.functionStart("toString", false, __FILE__, __LINE__);
      m_debug.debug("m_value = " + std::to_string(m_value));
      std::string result = toString(m_value, with_quotes);
      m_debug.functionEnd("toString: " + result);
      return result;
    }

    inline int POS() {
      m_debug.functionStart("POS", false, __FILE__, __LINE__);
      int i = m_value;
      m_debug.functionEnd("POS: " + std::to_string(i));
      return i;
    }

    int getValue() {
      return m_value;
    }
    
    std::string IMAGE(T r) {
      int index = enum_position(r);
      return valueArray.get(index).s;
    }

    int ToInt() {
      return m_value;
    }

    /*
      Emulation of VHDL functions
    */
    
    
    inline std::string IMAGE(Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& r) {return r.toString();}

    template<class X>
    inline std::string IMAGE(X& other) {
      return other.toString();
    }
    
    inline int LENGTH() { return m_right - m_left + 1; }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> LEFT() {
      m_debug.functionStart("LENGTH");
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = m_left;
      m_debug.functionEnd("LENGTH: " + std::to_string(e.m_value));
      return e;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> RIGHT() { Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e; e.m_value = m_right; return e; }
    inline bool ASCENDING() { return true; };
    inline int POS(char a) { return char_position(a); };
    inline int POS(T e) { return enum_position(e); };
    inline int POS(Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& r) {
      m_debug.functionStart("POS", false, __FILE__, __LINE__);
      int pos = r.POS();
      m_debug.functionEnd("POS: " + std::to_string(pos));
      return pos;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> VAL(int index) {
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = index;
      return e; };
    
    inline bool operator==(T e) const {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(e); return m_value == other.m_value;}
    inline bool operator!=(T e) const {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(e); return m_value != other.m_value;}
    inline bool operator==(char c) const {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(c); return m_value == other.m_value;}
    inline bool operator!=(char c) const {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(c); return m_value != other.m_value;}
    inline bool operator==(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) const { return m_value == other.m_value; }
    inline bool operator!=(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) const { return m_value != other.m_value; }

    std::string info() const {
      return "Enum: m_left = " + std::to_string(m_left) + ", m_right = " + std::to_string(m_right) + ", m_value = " + std::to_string(m_value);
    }
    
  };


}

#endif
