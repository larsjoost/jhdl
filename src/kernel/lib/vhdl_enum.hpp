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
    std::string s;
  };

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
  
  template <typename T, class E>
  class Enumeration {


    int* char_position_lookup() {
      const static E valueArray;
      const static int SIZE =  256;
      int* a = new int[SIZE];
      std::fill_n(a, SIZE, -1);
      for (int i=0; i<valueArray.size; i++) {
        if (valueArray.array[i].c != 0) {
          a[valueArray.array[i].c] = i;
        }
      }
      return a;
    }

    int underlying_index(T e) {
      return static_cast<std::underlying_type_t<T>>(e);
    }
    
    int* enum_position_lookup() {
      const static E valueArray;
      const static int SIZE =  valueArray.enum_size;
      int* a = new int[SIZE];
      for (int i=0; i<valueArray.size; i++) {
        if (valueArray.array[i].c == 0) {
          int index = underlying_index(valueArray.array[i].e);
          a[index] = i;
        }
      }
      return a;
    }

    int char_position(char c) {
      static int* l = char_position_lookup();
      return l[(int)c];
    }

    int enum_position(T e) {
      static int* l = enum_position_lookup();
      int index = underlying_index(e);
      return l[index];
    }

    EnumerationElement<T> position(int i) {
      const static E value_array;
      return value_array.array[i];
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
      const static E value_array;
      return value_array.size;
    }
    
  protected:

    void set(char c) {
      if (char_position(c) < 0) {
        std::cerr << "Assigned char value " << c << " not allowed" << std::endl;
      } else {
        m_value = char_position(c);
      }
    }
    void set(T e) {
      m_value = enum_position(e);
    }
    
  public:
    int m_value = 0;
    int m_left = 0;
    int m_right = 0;
    
    Enumeration() {
      m_left = 0;
      m_right = size() - 1;
    }
    
    Enumeration(T v) {
      set(v);
      m_left = 0;
      m_right = size() - 1;
    }

    Enumeration(Enumeration<T, E>& left, Enumeration<T, E>& right) {
      m_left = left.m_value;
      m_right = right.m_value;
    }
    
    void construct(const Enumeration<T, E>& other) {
      m_left = other.m_left;
      m_right = other.m_right;
    };

    void operator=(T v) {
      set(v);
    }

    void operator=(int v) {
      m_value = v;
    }
    
    void operator=(char c) {
      set(c);
    }

    void operator=(bool v) {
      m_value = v ? (T)1 : (T)0;
    }

    bool operator!() {
      return m_value != 0;
    }
      
    operator bool() const {
      return m_value != 0;
    }

    const std::string toString(bool with_quotes = true) const {
      Debug<true> debug = Debug<true>("Enumeration");
      debug.functionStart("toString");
      const static E valueArray;
      if (valueArray.array[m_value].c == 0) {
        return valueArray.array[m_value].s;
      }
      std::string s = with_quotes ? "'" : "";
      std::string result = s + std::string(1, valueArray.array[m_value].c) + s;
      debug.functionEnd("toString: " + result);
      return result;
    }

    inline int POS() {
      Debug<true> debug = Debug<true>("Enumeration");
      debug.functionStart("POS");
      int i = m_value;
      debug.functionEnd("POS: " + std::to_string(i));
      return i;
    }

    int getValue() {
      return m_value;
    }
    
    std::string IMAGE(T r) {
      const static E valueArray;
      int index = enum_position(r);
      return valueArray.array[index].s;
    }

    int ToInt() {
      return m_value;
    }

    /*
      Emulation of VHDL functions
     */
    
    
    inline std::string IMAGE(Enumeration<T, E>& r) {return r.toString();}

    template<class X>
    inline std::string IMAGE(X& other) {
      return other.toString();
    }
    
    inline int LENGTH() { return m_right - m_left + 1; }
    inline Enumeration<T, E> LEFT() {
      Debug<true> debug = Debug<true>("Enumeration");
      debug.functionStart("LENGTH");
      Enumeration<T, E> e;
      e.m_value = m_left;
      debug.functionEnd("LENGTH: " + std::to_string(e.m_value));
      return e;
    }
    inline Enumeration<T, E> RIGHT() { Enumeration<T, E> e; e.m_value = m_right; return e; }
    inline bool ASCENDING() { return true; };
    inline int POS(char a) { return char_position(a); };
    inline int POS(T e) { return enum_position(e); };
    inline int POS(Enumeration<T, E>& r) {
      Debug<true> debug = Debug<true>("Enumeration");
      debug.functionStart("POS");
      int pos = r.POS();
      debug.functionStart("POS: " + std::to_string(pos));
      return pos;
    }
    inline Enumeration<T, E> VAL(int index) { Enumeration<T, E> e; e.m_value = index; return e; };
    
    inline bool operator==(T e) const {Enumeration<T, E> other; other.set(e); return m_value == other.m_value;}
    inline bool operator!=(T e) const {Enumeration<T, E> other; other.set(e); return m_value != other.m_value;}
    inline bool operator==(char c) const {Enumeration<T, E> other; other.set(c); return m_value == other.m_value;}
    inline bool operator!=(char c) const {Enumeration<T, E> other; other.set(c); return m_value != other.m_value;}
    inline bool operator ==(const Enumeration<T, E> &other) const { return m_value == other.m_value; }
    inline bool operator !=(const Enumeration<T, E> &other) const { return m_value != other.m_value; }

    std::string info() const {
      return "Enum: m_left = " + std::to_string(m_left) + ", m_right = " + std::to_string(m_right) + ", m_value = " + std::to_string(m_value);
    }
    
  };

}

#endif
