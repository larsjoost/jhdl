#include <cassert>
#include <string>
#include <iostream>

#include "object_type.hpp"

namespace ast {

  std::string toString(ObjectType o) {
    static std::string a[NUMBER_OF_OBJECT_TYPES] = {
      "signal", "variable", "constant", "function", "procedure", "port", "type",
      "enum", "entity", "configuration", "subtype", "file", "group", "architecture",
      "package", "package body", "component"};
    return a[o];
  }
  
  std::string toString(ObjectValue o) {
    static std::string a[NUMBER_OF_OBJECT_VALUES] = {
      "integer", "real", "number", "character", "text",
      "physical", "array", "enumeration", "boolean",
      "user_type", "unknown", "none", "dont_care"};
    return a[o];
  }

  std::string ObjectValueContainer::toString() {
    if (value == USER_TYPE) {
      return typeName + "(User type)";
    }
    if (value == ARRAY) {
      return "array of " + subtype->toString();
    }
    if (value == ENUMERATION) {
      return "enumeration " + typeName;
    }
    return ast::toString(value);
  }

  bool ObjectValueContainer::numberEquals(ObjectValue l, ObjectValue r) {
    return (l == NUMBER) && (r == INTEGER || r == REAL || r == NUMBER);
  }
  
  bool ObjectValueContainer::equals(ObjectValueContainer& other) {
    bool verbose = false;
    if (verbose) {
      std::cout << "this = " + toString() << std::endl;
      std::cout << "other = " + other.toString() << std::endl;
    }
    if (numberEquals(value, other.value)) return true;
    if (numberEquals(other.value, value)) return true;
    if (value != other.value) return false;
    if (value == USER_TYPE || value == ENUMERATION) {
      return typeName == other.typeName;
    }
    return true;
  }

  int ObjectArguments::match(ObjectArguments& interface, ObjectArgument& association, int index, bool verbose) {
    auto it = interface.list.begin();
    if (verbose) {
      std::cout << "association.name.empty() " << (association.name.empty() ? "true" : "false") << std::endl;
    }
    if (association.name.empty() && index >= 0 && interface.list.size() > index) {
      std::advance(it, index);
      if (verbose) {
        std::cout << "it->type = " + it->type.toString() << std::endl;
        std::cout << "association.type = " + association.type.toString() << std::endl;
      }
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

  void ObjectArguments::setDefaultValues(bool m[], ObjectArguments& interface) {
    int index = 0;
    for (auto& i : interface.list) {
      if (i.defaultValue.empty()) {
        m[index] = false;
      } else {
        m[index] = true;
      }
      index++;
    }
  }
  
  /*
   *    interface      association       
   *    a : type1      a : type1   
   *    b : type2      c : type3   
   *    c : type3                  
   */
  
  bool ObjectArguments::equals(ObjectArguments& other) {
    bool verbose = false;
    ObjectArguments& interface = isInterface ? *this : other;
    ObjectArguments& association = isInterface ? other : *this;
    if (verbose) {
      std::cout << "Interface = " << interface.toString() << std::endl;
      std::cout << "Association = " << association.toString() << std::endl;
    }
    int size = interface.list.size();
    bool m[size];
    setDefaultValues(m, interface);
    int index = 0;
    for (ObjectArgument& a : association.list) {
      int i = match(interface, a, index, verbose);
      if (verbose) {
        std::cout << "i = " << std::to_string(i) << std::endl;
        std::cout << "index = " << std::to_string(index) << std::endl;
      }
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
