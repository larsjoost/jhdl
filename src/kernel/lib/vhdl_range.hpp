#ifndef VHDL_RANGE_HPP
#define VHDL_RANGE_HPP

#include <cstdlib>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <type_traits>

#include <systemc.h>

#include "../../debug/debug.hpp"

namespace vhdl {

  /*
   * Range examples:
   *     
   * type INTEGER is range -2147483647 to 2147483647;
   * type REAL is range -1.7014111e+308 to 1.7014111e+308;
   */
  
  template<class TYPE>
  class Range {

  protected:
    
    std::string m_name;
    std::string m_file_name;
    int m_line_number;
 
    TYPE m_value;
    TYPE m_left;
    TYPE m_right;
    bool m_ascending;

  public:
    
    Range<TYPE>(const char* name, const char* file_name, int line_number) : m_name(name), m_file_name(file_name), m_line_number(line_number){};
    Range<TYPE>() {}
    Range<TYPE>(TYPE left, TYPE right, bool ascending = true) {
      m_left = left;
      m_right = right;
      m_ascending = ascending;
    }
    Range<TYPE>(TYPE value) {
      m_value = value;
    }
    
    inline void construct(const Range<TYPE>& other) {
      m_left = other.m_left;
      m_right = other.m_right;
      m_ascending = other.m_ascending;
    };

    inline void construct(TYPE left, TYPE right, bool ascending = true) {
      Debug<true> debug = Debug<true>(this);
      debug.functionStart("construct(left = " + std::to_string(left) + ", right = " + std::to_string(right) +
			  ", ascending = " + std::to_string(ascending) + ")");
      m_left = left;
      m_right = right;
      m_ascending = ascending;
      debug.functionEnd("construct");
    };
    
    void construct() {
      // TODO: Implement
    }

    void init(TYPE value) {
      m_value = value;
    }
    
    inline void operator=(TYPE other) { m_value = other; }
    inline void operator=(Range<TYPE>& other) { m_value = other.m_value; }

    inline bool operator == (const Range<TYPE> &other) const { return m_value == other.m_value; }
    inline bool operator != (const Range<TYPE> &other) const { return m_value != other.m_value; }
    inline bool operator == (const TYPE other) const { return m_value == other; }
    inline bool operator <= (const TYPE other) const { return m_value <= other; }
    inline bool operator >= (const TYPE other) const { return m_value >= other; }
    inline bool operator != (const TYPE other) const { return m_value != other; }
    inline TYPE operator +  (const TYPE other) const { return m_value + other; }
    inline TYPE operator -  (const TYPE other) const { return m_value - other; }
    inline TYPE operator +  (const Range<TYPE>& other) const { return m_value + other.m_value; }
    inline TYPE operator -  (const Range<TYPE>& other) const { return m_value - other.m_value; }

    /* Required by sc_signals */
    inline friend std::ostream& operator << ( std::ostream& os,  Range<TYPE> const &other) {
      os << other.toString();
      return os;
    }

    inline friend void sc_trace(sc_trace_file *tf, const Range<TYPE> & v,
    const std::string & NAME ) {
      sc_trace(tf,v.m_value, NAME + ".value");
    }
    
    inline operator bool() const {
      return m_value != TYPE(0);
    }
    
    inline operator int() const {
      return m_value;
    }

    inline int ToInt() {
      return m_value;
    }
    
    inline const std::string toString() const {
      return std::to_string(m_value);
    }

    inline unsigned int LENGTH() {
      return HIGH() - LOW() + 1;
    }

    inline TYPE HIGH() {
      return (m_left > m_right ? m_left : m_right);
    }
    inline TYPE LOW() {
      return (m_left < m_right ? m_left : m_right);
    }
    inline TYPE LEFT() {
      return m_left;
    }
    inline TYPE RIGHT() {
      return m_right;
    }
    inline TYPE LEFTOF() {
      return (m_left < m_right ? getValue() - 1 : getValue() + 1);
    }
    inline TYPE RIGHTOF() {
      return (m_left < m_right ? getValue() + 1 : getValue() - 1);
    }

    inline TYPE POS() {
      return m_value;
    }

    inline TYPE POS(Range<TYPE>& other) {
      return other.POS();
    }

    inline bool ASCENDING() {
      return m_ascending;
    }
    
    inline int getValue() {
      return m_value;
    }

    inline std::string STATUS() {
      return toString();
    }
    
    template <class T>
    std::string IMAGE(T& r) {
      return r.toString();
    }

    inline std::string IMAGE(TYPE r) {
      return std::to_string(r);
    }
    
    std::string info() const {
      return "Range: m_left = " + std::to_string(m_left) + ", m_right = " + std::to_string(m_right) + ", m_value = " + std::to_string(m_value);
    }

  };

  
}

#endif
