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
      "INTEGER", "REAL", "NUMBER", "BOOLEAN", "CHARACTER", "TEXT",
      "PHYSICAL", "ARRAY", "USER_TYPE", "UNKNOWN", "NONE", "DONT_CARE"};
    return a[o];
  }

  std::string ObjectValueContainer::toString() {
    if (value == USER_TYPE) {
      return typeName;
    }
    return ast::toString(value);
  }

  bool ObjectValueContainer::equals(ObjectValueContainer& other) {
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
    if (association.name.empty() && index >= 0 && interface.list.size() > index) {
      std::advance(it, index);
      return it->type.equals(association.type) ? index : -1;
    } else {
      for (int i = 0; i < interface.list.size(); i++) {
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
    //    std::cout << "Interface = " << interface.toString() << std::endl;
    //   std::cout << "Association = " << association.toString() << std::endl;
    int size = interface.list.size();
    bool m[size] {};
    int index = 0;
    for (ObjectArgument& a : association.list) {
      int i = match(interface, a, index);
      // std::cout << "i = " << std::to_string(i) << std::endl;
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
