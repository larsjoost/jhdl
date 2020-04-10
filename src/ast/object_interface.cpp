#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "object_type.hpp"

namespace ast {

  int ObjectInterface::match(const ObjectArgument& association, int index) {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("match(association = " + association.toString(true) +
			", index = " + std::to_string(index) + ")", false, __FILE__, __LINE__);
    int result = -1;
    auto it = m_elements.begin();
    debug.debug("association.name.empty() " + std::string(association.m_name.empty() ? "true" : "false"));
    if (association.m_name.empty() && index >= 0 && m_elements.size() > index) {
      std::advance(it, index);
      result = (association.m_types.equals(it->m_type) ? index : -1);
    } else {
      for (int i = 0; i < m_elements.size(); i++, it++) {
	debug.debug("interface name = " + it->m_name + ", association name = " + association.m_name);
	if (it->m_name == association.m_name) {
	  result = (association.m_types.equals(it->m_type) ? i : -1);
	  break;
        }
      }
    }
    debug.functionEnd("match: " + std::to_string(result));
    return result;
  }


  /*
   *    interface      association       
   *    a : type1      a : type1   
   *    b : type2      c : type3   
   *    c : type3                  
   */
  
  bool ObjectInterface::matches(ObjectArguments& association) {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("matches(this = " + toString() + ", association = " + association.toString() + ")", false, __FILE__, __LINE__);
    bool result = true;
    debug.debug("Interface = " + toString());
    debug.debug("Association = " + association.toString());
    int size = m_elements.size();
    bool m[size];
    int index = 0;
    for (auto& i : m_elements) {
      m[index] = i.m_default_value ? true : false;
      index++;
    }
    index = 0;
    for (const ObjectArgument& a : association.getList()) {
      int i = match(a, index);
      debug.debug("i = " + std::to_string(i));
      debug.debug("index = " + std::to_string(index));
      if (i < 0) {
	result = false;
	break;
      }
      if (index >= 0) {
        if (i == index) {
          index++;
        } else {
          index = -1;
        }
      }
      m[i] = true;
    }
    if (result) {
      for (int i=0; i<size; i++) {
	if (!m[i]) {
	  result = false;
	  break;
	}
      }
    }
    debug.functionEnd("matches: " + std::to_string(result));
    return result;
  }

  std::string ObjectInterface::toString(bool verbose) {
    std::string delimiter = "";
    std::string s;
    for (auto& a : m_elements) {
      s += delimiter + a.toString(verbose);
      delimiter = ", ";
    }
    return s;
  }

  std::string ObjectInterfaceElement::toString(bool verbose) {
    return m_name + (m_type_name.empty() ? " " : " : " + m_type_name) + "(" + m_type.toString(verbose) + ")" + (m_default_value ? " := [DEFAULT]" : "");
  }
  
}

 
