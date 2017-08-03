#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "object_type.hpp"

namespace ast {

  std::string toString(ObjectType o) {
    switch (o) {
    case ObjectType::SIGNAL: return "signal";
    case ObjectType::VARIABLE: return "variable";
    case ObjectType::CONSTANT: return "constant";
    case ObjectType::FUNCTION: return "function";
    case ObjectType::PROCEDURE: return "procedure";
    case ObjectType::PORT: return "port";
    case ObjectType::TYPE: return "type";
    case ObjectType::ENUM: return "enum";
    case ObjectType::ENTITY: return "entity";
    case ObjectType::CONFIGURATION: return "configuration";
    case ObjectType::SUBTYPE: return "subtype";
    case ObjectType::FILE: return "file";
    case ObjectType::GROUP: return "group";
    case ObjectType::ARCHITECTURE: return "architecture";
    case ObjectType::PACKAGE: return "package";
    case ObjectType::PACKAGE_BODY: return "package_body";
    case ObjectType::COMPONENT: return "component";
    default: assert(false);
    }
  }
  
  std::string toString(ObjectValue o) {
    switch (o) {
    case ObjectValue::INTEGER: return "integer"; 
    case ObjectValue::REAL: return "real";
    case ObjectValue::NUMBER: return "number";
    case ObjectValue::CHARACTER: return "character";
    case ObjectValue::TEXT: return "text";
    case ObjectValue::PHYSICAL: return "physical";
    case ObjectValue::ARRAY: return "array";
    case ObjectValue::ENUMERATION: return "enumeration";
    case ObjectValue::ACCESS: return "access";
    case ObjectValue::FILE: return "file";
    case ObjectValue::BOOLEAN: return "boolean";
    case ObjectValue::USER_TYPE: return "user_type";
    case ObjectValue::UNKNOWN: return "unknown";
    case ObjectValue::NONE: return "none";
    case ObjectValue::DONT_CARE: return "dont_care";
    default: assert(false);
    };
  }

  std::string ObjectValueContainer::toString() {
    if (value == ObjectValue::USER_TYPE) {
      return typeName + "(User type)";
    }
    if (value == ObjectValue::ARRAY) {
      return "array of " + subtype->toString();
    }
    if (value == ObjectValue::ENUMERATION) {
      return "enumeration " + typeName;
    }
    return ast::toString(value);
  }

  bool ObjectValueContainer::numberEquals(ObjectValue l, ObjectValue r) {
    return
      (l == ObjectValue::NUMBER) &&
      (r == ObjectValue::INTEGER || r == ObjectValue::REAL || r == ObjectValue::NUMBER);
  }
  
  bool ObjectValueContainer::equals(ObjectValueContainer& other) {
    bool verbose = false;
    bool result;
    if (numberEquals(value, other.value) || numberEquals(other.value, value)) {
      result = true;
    } else if (value != other.value) {
      result = false;
    } else if (value == ObjectValue::USER_TYPE ||
               value == ObjectValue::ENUMERATION) {
      result = (typeName == other.typeName);
    } else {
      result = true;
    }
    if (verbose) {
      std::cout << "ObjectValueContainer::equals: " << toString() << (result ? " == " : " != ") << other.toString() << std::endl;
    }
    return result;
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
  
  bool ObjectArguments::equals(ObjectArguments& other, bool verbose) {
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

  bool ObjectArguments::ExactMatch(ObjectArguments& other) {
    Debug<false> debug = Debug<false>("ObjectArgument");
    debug.functionStart("ExactMatch");
    bool result = true;
    if (size() == other.size()) {
      auto l = list.begin();
      auto r = other.list.begin();
      while (l != list.end() && r != other.list.end()) {
        if (l->type_name != r->type_name) {
          result = false;
          break;
        }
        l++;
        r++;
      }
    } else {
      result = false;
    }
    debug.debug("(" + toString() + ")" + (result ? " == " : " /= ") + "(" + other.toString() + ")",
                true, result ? Output::Color::GREEN : Output::Color::RED);
    debug.functionEnd("ExactMatch");
    return result;
  }

  std::string ObjectArgument::toString() {
    return name + " : " + type_name + "(" + type.toString() + ") := " + defaultValue;
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

 
