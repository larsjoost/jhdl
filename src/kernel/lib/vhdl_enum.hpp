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
    const std::string toString() const {
      return std::string(1, c) + "(" + std::to_string(c) + ") : " + std::string(s);
    }
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
    
    template<int N>
    struct Array {
      int table[N];
    };

    static bool lookup_tables_initialized;
    static EnumerationElement<T> value_array[SIZE];
    static Array<ENUM_SIZE> enum_position_lookup;
    static Array<CHAR_SIZE> char_position_lookup;

    int underlying_index(T e) const {
      return static_cast<std::underlying_type_t<T>>(e);
    }

    void value_array_initialization() {
      m_debug.functionStart("value_array_initialization", false, __FILE__, __LINE__);
      E v;
      for (int i=0; i<SIZE; i++) {
	m_debug.debug("v.table[" + std::to_string(i) + "] = " + v.table[i].toString());
	value_array[i] = v.table[i];
	m_debug.debug("value_array[" + std::to_string(i) + "] = " + value_array[i].toString());
      }
      m_debug.functionEnd("value_array_initialization");
    }
    
    void enum_position_lookup_initialization(Array<ENUM_SIZE>& lut) {
      m_debug.functionStart("enum_position_lookup_initialization", false, __FILE__, __LINE__);
      for (int i=0; i<ENUM_SIZE; i++) {
	lut.table[i] = -1;
      }
      for (int i=0; i<SIZE; i++) {
	EnumerationElement<T>& element = value_array[i];
	m_debug.debug("Element = " + element.toString());
	if (element.c == 0) {
	  int index = underlying_index(element.e);
	  assert(index < ENUM_SIZE);
	  m_debug.debug("lut.table[" + std::to_string(index) + "] = " + std::to_string(i));
	  lut.table[index] = i;
	}
      }
      m_debug.functionEnd("enum_position_lookup_initialization");
    }

    void char_position_lookup_initialization(Array<CHAR_SIZE>& lut) {
      for (int i=0; i<CHAR_SIZE; i++) {
	lut.table[i] = -1;
      }
      for (int i=0; i<SIZE; i++) {
	EnumerationElement<T>& element = value_array[i];
	int index = (int)element.c;
	if (index >= CHAR_SIZE) {
	  std::string message = "valueArray[" + std::to_string(i) + "].c = " + std::to_string(index) + " > " + std::to_string(CHAR_SIZE);
	  assert(false && message.c_str());
	} else if (index != 0) {
	  lut.table[index] = i;
	}
      }
    }
   
    int char_position(char c) {
      return char_position_lookup.table[(int)c];
    }
    
    int enum_position(T e) const {
      /* m_debug.functionStart("enum_position", false, __FILE__, __LINE__);
      if (m_debug.isVerbose()) {
	std::string s = "{";
	std::string delimiter; 
	for (int i=0; i<ENUM_SIZE; i++) {
	  s += delimiter + std::to_string(enum_position_lookup.table[i]);
	  delimiter = ", ";
	}
	s += "}";
	m_debug.debug("enum_position_lookup = " + s);
      }
      */
      int index = underlying_index(e);
      // m_debug.debug("index = " + std::to_string(index));
      int result = enum_position_lookup.table[index];
      // m_debug.functionEnd("enum_position: " + std::to_string(result));
      return result;
    }

    EnumerationElement<T> position(int i) {
      return value_array[i];
    }

    int position(EnumerationElement<T>& e) {
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

    bool initialize() {
      if (!lookup_tables_initialized) {
	value_array_initialization();
	enum_position_lookup_initialization(enum_position_lookup);
	char_position_lookup_initialization(char_position_lookup);
	lookup_tables_initialized = true;
      }
      return lookup_tables_initialized;
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
    int m_right = SIZE - 1;

    Enumeration(const char* name) : m_debug(this), m_name(name) {
      m_debug.functionStart("Enumeration(char c = " + std::string(name) + ")", false, __FILE__, __LINE__);
      m_debug.functionEnd("Enumeration");
    };
 
    Enumeration() : m_debug(this) {
      m_debug.functionStart("Enumeration", false, __FILE__, __LINE__);
      m_debug.functionEnd("Enumeration");
    }
    
    Enumeration(T v) : m_debug(this) {
      m_debug.functionStart("Enumeration(v = " + toString(v) + ")", false, __FILE__, __LINE__);
      set(v);
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
    
    Enumeration(int i) : m_debug(this) {
      m_debug.functionStart("Enumeration(int i = " + std::to_string(i) + ")", false, __FILE__, __LINE__);
      m_value = i;
      m_debug.functionEnd("Enumeration");
    }
    void constrain(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& other) {
      m_left = other.m_left;
      m_right = other.m_right;
    };

    void constrain(int left, int right, int ascending = true) {
      m_left = left;
      m_right = right;
    };

    template<typename T1, typename T2>
    void constrain(T1 left, T2 right, int ascending = true) {
      m_left = POS(left);
      m_right = POS(right);
    }
    
    void constrain() {
      m_left = 0;
      m_right = SIZE - 1;
    };

    
    
    void operator=(T v) {
      m_debug.functionStart("operator=", false, __FILE__, __LINE__);
      set(v);
      m_debug.functionEnd("operator=");
    }

    inline void operator=(int v) {
      m_value = v;
    }
    
    inline void operator=(char c) {
      m_debug.functionStart("operator=(char c = " + std::string(1, c) + ")");
      set(c);
      m_debug.functionEnd("operator=");
    }

    inline void operator=(bool v) {
      m_value = v ? (T)1 : (T)0;
    }

    inline void operator=(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) { m_value = other.m_value; }
    
    inline bool operator!() {
      return m_value != 0;
    }
      
    inline operator bool() const {
      return m_value != 0;
    }

    const std::string toString(int index, bool with_quotes = true) const {
      // m_debug.functionStart("toString(index = " + std::to_string(index) + ")", false, __FILE__, __LINE__);
      assert(lookup_tables_initialized);
      std::string result;
      if (index < SIZE) {
	if (value_array[index].c == 0) {
	  result = value_array[index].s;
	} else {
	  std::string s = with_quotes ? "'" : "";
	  result = s + std::string(1, value_array[index].c) + s;
	}
      } else {
	result = "UNKNOWN";
      }
      // m_debug.functionEnd("toString: " + result);
      return result;
    }

    const std::string toString(T e, bool with_quotes = true) const {
      // m_debug.functionStart("toString", false, __FILE__, __LINE__);
      int index = enum_position(e);
      // m_debug.debug("index = " + std::to_string(index));
      const std::string result = toString(index, with_quotes);
      // m_debug.functionEnd("toString: " + result);
      return result;
    }

    const std::string toString(bool with_quotes = true) const {
      // m_debug.functionStart("toString", false, __FILE__, __LINE__);
      // m_debug.debug("m_value = " + std::to_string(m_value));
      const std::string result = toString(m_value, with_quotes);
      // m_debug.functionEnd("toString: " + result);
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
    
    int ToInt() {
      return m_value;
    }

    /*
      Emulation of VHDL functions
    */

    std::string IMAGE(T r) {
      m_debug.functionStart("IMAGE", false, __FILE__, __LINE__);
      int index = enum_position(r);
      m_debug.functionEnd("IMAGE");
      return value_array[index].s;
    }
    
    template<typename X>
    inline std::string IMAGE(X& other) {
      m_debug.functionStart("IMAGE", false, __FILE__, __LINE__);
      m_debug.functionEnd("IMAGE");
      return other.toString();
    }
    
    inline std::string IMAGE(char c) {
      m_debug.functionStart("IMAGE(c = " + std::to_string(c) + ")", false, __FILE__, __LINE__);
      std::string result = std::string(1, c);
      m_debug.functionEnd("IMAGE: " + result);
      return result;
    }
    
    inline int LENGTH() { return m_right - m_left + 1; }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> LEFT() {
      m_debug.functionStart("LENGTH");
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = m_left;
      m_debug.functionEnd("LENGTH: " + std::to_string(e.m_value));
      return e;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> LEFTOF() {
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = m_value - 1;
      return e;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> LOW() {
      return LEFT();
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> RIGHT() {
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = m_right;
      return e;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> RIGHTOF() {
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = m_value + 1;
      return e;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> HIGH() {
      return RIGHT();
    }
    inline bool ASCENDING() { return true; };
    inline int relative_position(int i) { return i - m_left; }
    inline int POS(char a) { return relative_position(char_position(a)); };
    inline int POS(T e) { return relative_position(enum_position(e)); };
    inline int POS(Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>& r) {
      m_debug.functionStart("POS", false, __FILE__, __LINE__);
      int pos = relative_position(r.POS());
      m_debug.functionEnd("POS: " + std::to_string(pos));
      return pos;
    }
    inline Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> VAL(int index) {
      Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> e;
      e.m_value = index;
      return e; };
    
    // inline bool operator==(T e) const {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(e); return m_value == other.m_value;}
    // inline bool operator!=(T e) {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(e); return m_value != other.m_value;}
    inline bool operator==(char c) {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(c); return m_value == other.m_value;}
    inline bool operator!=(char c) {Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> other; other.set(c); return m_value != other.m_value;}
    inline bool operator==(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) { return m_value == other.m_value; }
    inline bool operator<=(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) { return m_value <= other.m_value; }
    inline bool operator>=(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) { return m_value >= other.m_value; }
    inline bool operator!=(const Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE> &other) { return m_value != other.m_value; }

    std::string info() {
      return "Enum<" + std::to_string(m_left) + "(" + toString(m_left) + ") to " + std::to_string(m_right) + "(" + toString(m_right) + ") = " +
	std::to_string(m_value) + "(" + toString() + ")>";
    }
    
  };

  template <typename T, class E, int SIZE, int ENUM_SIZE, int CHAR_SIZE>
  bool Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>::lookup_tables_initialized = Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>().initialize();

  template <typename T, class E, int SIZE, int ENUM_SIZE, int CHAR_SIZE>
  EnumerationElement<T> Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>::value_array[SIZE];

  template <typename T, class E, int SIZE, int ENUM_SIZE, int CHAR_SIZE>
  Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>::Array<ENUM_SIZE> Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>::enum_position_lookup;

  template <typename T, class E, int SIZE, int ENUM_SIZE, int CHAR_SIZE>
  Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>::Array<CHAR_SIZE> Enumeration<T, E, SIZE, ENUM_SIZE, CHAR_SIZE>::char_position_lookup;
}

#endif
