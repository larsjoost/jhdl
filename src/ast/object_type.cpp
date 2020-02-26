#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "object_type.hpp"

namespace ast {

  std::string toString(ObjectType o) {
    std::string result;
    switch (o) {
    case ObjectType::SIGNAL: result = "signal"; break;
    case ObjectType::VARIABLE: result =  "variable"; break;
    case ObjectType::CONSTANT: result =  "constant"; break;
    case ObjectType::FUNCTION: result =  "function"; break;
    case ObjectType::PROCEDURE: result =  "procedure"; break;
    case ObjectType::PORT: result =  "port"; break;
    case ObjectType::TYPE: result =  "type"; break;
    case ObjectType::ENUM: result =  "enum"; break;
    case ObjectType::ENTITY: result =  "entity"; break;
    case ObjectType::CONFIGURATION: result =  "configuration"; break;
    case ObjectType::SUBTYPE: result =  "subtype"; break;
    case ObjectType::FILE: result =  "file"; break;
    case ObjectType::GROUP: result =  "group"; break;
    case ObjectType::ARCHITECTURE: result =  "architecture"; break;
    case ObjectType::GENERATE: result =  "generate"; break;
    case ObjectType::PACKAGE: result =  "package"; break;
    case ObjectType::PACKAGE_BODY: result =  "package_body"; break;
    case ObjectType::COMPONENT: result =  "component"; break;
    case ObjectType::BLOCK: result =  "block"; break;
    case ObjectType::PROCESS: result =  "process"; break;
    case ObjectType::FACTORY: result =  "factory"; break;
    case ObjectType::UNKNOWN: result =  "unknown"; break;
    default: assert(false);
    }
    return result;
  }
  
  std::string toString(ObjectValue o) {
    switch (o) {
    case ObjectValue::INTEGER: return "integer"; 
    case ObjectValue::REAL: return "real";
    case ObjectValue::NUMBER: return "number";
    case ObjectValue::CHARACTER: return "character";
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

  std::string ObjectValueContainer::ToString(const Array& l, bool verbose) const {
    std::string subtype;
    std::string delimiter;
    for (const ObjectValueContainer& s : l) {
      subtype += delimiter + s.toString(verbose);
      delimiter = ", ";
    }
    return subtype;
  }
  
  std::string ObjectValueContainer::toString(bool verbose) const {
    if (verbose) {
      return "value = " + ast::toString(a_value) + "type name = \"" + a_type_name + "\", subtype = (" + ToString(a_subtype, verbose) + ")";
    } else {
      if (a_value == ObjectValue::USER_TYPE) {
        return a_type_name + "(User type)";
      }
      if (a_value == ObjectValue::ARRAY) {
        assert(!a_subtype.empty());
        return "array(" + ToString(a_arguments) + ") of " + ToString(a_subtype);
      }
      if (a_value == ObjectValue::ENUMERATION) {
        return "enumeration " + a_type_name;
      }
      return ast::toString(a_value);
    }
  }

  bool ObjectValueContainer::numberEquals(ObjectValue l, ObjectValue r) const {
    return
      (l == ObjectValue::NUMBER) &&
      (r == ObjectValue::INTEGER || r == ObjectValue::REAL || r == ObjectValue::NUMBER);
  }

  bool ObjectValueContainer::equals(const Array& l, const Array& r) const {
    bool result = true;
    if (l.size() != r.size()) {
      result = false;
    } else {
      auto il = l.begin();
      auto ir = r.begin();
      for ( ; il != l.end(); il++, ir++) {
        if (!il->equals(*ir)) {
          result = false;
          break;
        } 
      }
    }
    return result;
  }

  bool ObjectValueContainer::equals(const ObjectValueContainer& other) const {
    bool verbose = false;
    bool result;
    if (a_value == ObjectValue::UNKNOWN || other.a_value == ObjectValue::UNKNOWN) {
      result = true;
    } else if (numberEquals(a_value, other.a_value) || numberEquals(other.a_value, a_value)) {
      result = true;
    } else if (a_value != other.a_value) {
      result = false;
    } else if (a_value == ObjectValue::USER_TYPE ||
               a_value == ObjectValue::ENUMERATION) {
      result = (a_type_name.empty() || other.a_type_name.empty() || a_type_name == other.a_type_name);
    } else if (HasArray(a_value)) {
      assert(!a_subtype.empty());
      assert(!other.a_subtype.empty());
      result = equals(a_subtype, other.a_subtype);
    } else {
      result = true;
    }
    if (verbose) {
      std::cout << "ObjectValueContainer::equals: " << toString() << (result ? " == " : " != ") << other.toString() << std::endl;
    }
    return result;
  }

  int ObjectArguments::match(ObjectArguments& interface, ObjectArgument& association, int index, bool verbose) {
    Debug<true> debug = Debug<true>("ObjectArguments::match");
    debug.setVerbose(verbose);
    debug.functionStart("match(interface =  " + interface.toString() + ", association = " + association.toString() +
			", index = " + std::to_string(index) + ")");
    int result = -1;
    auto it = interface.list.begin();
    debug.debug("association.name.empty() " + std::string(association.name.empty() ? "true" : "false"));
    if (association.name.empty() && index >= 0 && interface.list.size() > index) {
      std::advance(it, index);
      debug.debug("it->type = " + it->type.toString());
      debug.debug("association.type = " + association.type.toString());
      result = (it->type.equals(association.type) ? index : -1);
    } else {
      for (int i = 0; i < interface.list.size(); i++, it++) {
	debug.debug("interface name = " + it->name + ", association name = " + association.name);
	if (it->name == association.name) {
	  debug.debug("it->type = " + it->type.toString());
	  debug.debug("association.type = " + association.type.toString());
          result = (it->type.equals(association.type) ? i : -1);
	  break;
        }
      }
    }
    debug.functionEnd("match: " + std::to_string(result));
    return result;
  }

  void ObjectArguments::setDefaultValues(bool m[], ObjectArguments& interface) {
    int index = 0;
    for (auto& i : interface.list) {
      m[index] = i.default_value ? true : false;
      index++;
    }
  }
  
  /*
   *    interface      association       
   *    a : type1      a : type1   
   *    b : type2      c : type3   
   *    c : type3                  
   */
  
  bool ObjectArguments::equals(ObjectArguments& other, bool array_type, const bool verbose) {
    Debug<true> debug = Debug<true>("ObjectArguments::equals(ObjectArguments& other, bool array_type, bool verbose)");
    debug.setVerbose(verbose);
    debug.functionStart("equals(this = " + this->toString() + ", other = " + other.toString() +
			", array_type = " + std::to_string(array_type) + ")");
    ObjectArguments& interface   = isInterface() ? *this : other;
    ObjectArguments& association = isInterface() ? other : *this;
    bool result = true;
    debug.debug("Interface = " + interface.toString());
    debug.debug("Association = " + association.toString());
    int size = interface.list.size();
    bool m[size];
    setDefaultValues(m, interface);
    int index = 0;
    for (ObjectArgument& a : association.list) {
      int i = match(interface, a, index, verbose);
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
    if (result && !array_type) {
      for (int i=0; i<size; i++) {
	if (!m[i]) {
	  result = false;
	  break;
	}
      }
    }
    debug.functionEnd("equals: " + std::to_string(result));
    return result;
  }

  bool ObjectArguments::equals(ObjectValueContainer::Array& other, bool array_type, bool verbose) {
    ObjectArguments args(false);
    for (ObjectValueContainer& i : other) {
      ObjectArgument x(i);
      args.list.push_back(x);
    }
    return equals(args, array_type, verbose);
  }

  bool ObjectArguments::equals(const ObjectValueContainer& other, bool verbose) {
    if (verbose) { std::cout << "equals(other = " << other.toString() << ")" << std::endl; }
    ObjectArguments args(false);
    ObjectArgument x(other);
    args.list.push_back(x);
    return equals(args, false, verbose);
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
    return name + (type_name.empty() ? " " : " : " + type_name) + "(" + type.toString() + ")" + (default_value ? " := [DEFAULT]" : "");
  }
  
  std::string ObjectArguments::toString() {
    std::string delimiter = "";
    std::string s = "interface = " + std::string(isInterface() ? "true" : "false") + ": ";
    for (auto& a : list) {
      s += delimiter + a.toString();
      delimiter = ", ";
    }
    return s;
  }
  
}

 
