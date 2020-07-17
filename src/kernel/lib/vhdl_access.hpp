#ifndef VHDL_ACCESS_HPP
#define VHDL_ACCESS_HPP

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

#include "vhdl_exception.hpp"

#include "../../debug/debug.hpp"

namespace vhdl {

  template<class T>
  class vhdl_access {

    Debug<false> m_debug;
    Exceptions m_exceptions;

    std::string m_name;
    
    T m_value;

    bool m_null = true;
  
  public:

    vhdl_access(const char* name) : m_debug(this), m_name(name) {};

    T& ALL() {
      if (m_null) {
	m_exceptions.printError("Access type " + m_name + " ALL function called when contents is null",
				__FILE__, __LINE__);
	throw RuntimeError();
      }
      return m_value;
    }

    void constrain() {
      m_value.constrain();
    }
    
    void constrain(vhdl_access<T> s) {
      m_value.constrain(s.m_value);
    }
  
    bool isNull() {
      return m_null;
    };

    void DEALLOCATE() {
      m_null = true;
    }

    inline void push_back(std::string& s) {
      m_debug.functionStart("push_back(s = " + s + ")", false, __FILE__, __LINE__);
      m_null = false;
      m_value.push_back(s);
      m_debug.functionEnd("push_back");
    }
    
  };


  
  
}

#endif
