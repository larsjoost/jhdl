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
        a_value = char_position(c);
      }
    }
    void set(T e) {
      a_value = enum_position(e);
    }
    
  public:
    int a_value = 0;
    int a_left = 0;
    int a_right = 0;
    
    Enumeration() {
      a_left = 0;
      a_right = size() - 1;
    }
    
    Enumeration(T v) {
      set(v);
      a_left = 0;
      a_right = size() - 1;
    }

    Enumeration(Enumeration<T, E>& left, Enumeration<T, E>& right) {
      a_left = left.a_value;
      a_right = right.a_value;
    }
    
    void construct(const Enumeration<T, E>& other) {
      a_left = other.a_left;
      a_right = other.a_right;
    };

    void operator=(T v) {
      set(v);
    }

    void operator=(int v) {
      a_value = v;
    }
    
    void operator=(char c) {
      set(c);
    }

    void operator=(bool v) {
      a_value = v ? (T)1 : (T)0;
    }

    bool operator!() {
      return a_value != 0;
    }
      
    operator bool() const {
      return a_value != 0;
    }

    const std::string toString(bool with_quotes = true) const {
      Debug<true> debug = Debug<true>("Enumeration");
      debug.functionStart("toString");
      const static E valueArray;
      if (valueArray.array[a_value].c == 0) {
        return valueArray.array[a_value].s;
      }
      std::string s = with_quotes ? "'" : "";
      std::string result = s + std::string(1, valueArray.array[a_value].c) + s;
      debug.functionEnd("toString: " + result);
      return result;
    }

    inline int POS() {
      Debug<true> a_debug = Debug<true>("Enumeration");
      a_debug.functionStart("POS");
      int i = a_value;
      a_debug.functionEnd("POS: " + std::to_string(i));
      return i;
    }

    int getValue() {
      return a_value;
    }
    
    std::string IMAGE(T r) {
      const static E valueArray;
      int index = enum_position(r);
      return valueArray.array[index].s;
    }

    int ToInt() {
      return a_value;
    }

    /*
      Emulation of VHDL functions
     */
    
    inline int POS(Enumeration<T, E>& r) {return r.POS();}
    
    inline std::string IMAGE(Enumeration<T, E>& r) {return r.toString();}

    template<class X>
    inline std::string IMAGE(X& other) {
      return other.toString();
    }
    
    inline int LENGTH() { return 1; }
    inline Enumeration<T, E> LEFT() { Enumeration<T, E> e; e.a_value = a_left; return e; }
    inline Enumeration<T, E> RIGHT() { Enumeration<T, E> e; e.a_value = a_right; return e; }
    
    inline bool operator==(T e) const {Enumeration<T, E> other; other.set(e); return a_value == other.a_value;}
    inline bool operator!=(T e) const {Enumeration<T, E> other; other.set(e); return a_value != other.a_value;}
    inline bool operator==(char c) const {Enumeration<T, E> other; other.set(c); return a_value == other.a_value;}
    inline bool operator!=(char c) const {Enumeration<T, E> other; other.set(c); return a_value != other.a_value;}
    inline bool operator ==(const Enumeration<T, E> &other) const { return a_value == other.a_value; }
    inline bool operator !=(const Enumeration<T, E> &other) const { return a_value != other.a_value; }

  };

}

#endif
