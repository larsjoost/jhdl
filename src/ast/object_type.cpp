#include <cassert>
#include <string>
#include <iostream>

#include "object_type.hpp"

namespace ast {

  std::string toString(ObjectType o) {
    static std::string a[NUMBER_OF_OBJECT_TYPES] = {
      "SIGNAL", "VARIABLE", "CONSTANT", "FUNCTION", "PROCEDURE", "PORT", "TYPE",
      "ENUM", "ENTITY", "CONFIGURATION", "SUBTYPE", "FILE", "GROUP", "ARCHITECTURE",
      "PACKAGE", "PACKAGE BODY", "COMPONENT"};
    return a[o];
  }
  
  std::string toString(ObjectValue o) {
    static std::string a[NUMBER_OF_OBJECT_VALUES] = {
      "INTEGER", "REAL", "NUMBER", "CHARACTER", "TEXT",
      "PHYSICAL", "ARRAY", "ENUMERATION", "USER_TYPE", "UNKNOWN", "NONE", "DONT_CARE"};
    return a[o];
  }

  std::string ObjectValueContainer::toString() {
    if (value == USER_TYPE) {
      return typeName + "(User type)";
    }
    if (value == ARRAY) {
      return "ARRAY of " + subtype->toString();
    }
    return ast::toString(value);
  }

  bool ObjectValueContainer::equals(ObjectValueContainer& other) {
    // std::cout << "this = " + toString() << std::endl;
    // std::cout << "other = " + other.toString() << std::endl;
    if (value == NUMBER) return (other.value == INTEGER || other.value == REAL || other.value == NUMBER);
    if (other.value == NUMBER) return (value == INTEGER || value == REAL || value == NUMBER);
    if (value != other.value) return false;
    if (value == USER_TYPE) {
      return typeName == other.typeName;
    }
    return true;
  }

  int ObjectArguments::match(ObjectArguments& interface, ObjectArgument& association, int index) {
    auto it = interface.list.begin();
    // std::cout << "association.name.empty() " << (association.name.empty() ? "true" : "false") << std::endl;
    if (association.name.empty() && index >= 0 && interface.list.size() > index) {
      std::advance(it, index);
      // std::cout << "it->type = " + it->type.toString() << std::endl;
      // std::cout << "association.type = " + association.type.toString() << std::endl;
      return it->type.equals(association.type) ? index : -1;
    } else {
      for (int i = 0; i < interface.list.size(); i++, it++) {
        if (it->name == association.name) {
          return it->type.equals(association.type) ? i : -1;
        }
      }
    }
    return -1;
  }

  /*
   *    interface      association       
   *    a : type1      a : type1   
   *    b : type2      c : type3   
   *    c : type3                  
   */
  
  bool ObjectArguments::equals(ObjectArguments& other) {
    ObjectArguments& interface = isInterface ? *this : other;
    ObjectArguments& association = isInterface ? other : *this;
    // std::cout << "Interface = " << interface.toString() << std::endl;
    // std::cout << "Association = " << association.toString() << std::endl;
    int size = interface.list.size();
    bool m[size] {};
    int index = 0;
    for (ObjectArgument& a : association.list) {
      int i = match(interface, a, index);
      // std::cout << "i = " << std::to_string(i) << std::endl;
      // std::cout << "index = " << std::to_string(index) << std::endl;
      if (i < 0) {
        return false;
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
    for (int i=0; i<size; i++) {
      if (!m[i]) {
        return false;
      }
    }
    return true;
  }

  std::string ObjectArgument::toString() {
    return name + " : " + type.toString() + " := " + defaultValue;
  }
  
  std::string ObjectArguments::toString() {
    std::string delimiter = "";
    std::string s = "";
    for (auto& a : list) {
      s += delimiter + a.toString();
      delimiter = ", ";
    }
    return s;
  }
  
}
