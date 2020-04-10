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

  bool ObjectValueContainer::equals(const ObjectArguments& arguments) {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("equals(this = " + toString(true) + "; arguments = " + arguments.toString(true), false, __FILE__, __LINE__);
    bool result = true;
    if (a_arguments.size() >= arguments.size()) {
      auto il = a_arguments.begin();
      auto ir = arguments.getList().begin();
      for ( ; ir != arguments.getList().end(); il++, ir++) {
        if (!ir->equals(*il)) {
          result = false;
          break;
        } 
      }
    } else {
      debug.debug("(a_arguments.size() = " + std::to_string(a_arguments.size()) + ") != (other.size() = " + std::to_string(arguments.size()) + ")"); 
    }
    debug.functionEnd("equals: " + std::to_string(result));
    return result;
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
      return "a_value = " + ast::toString(a_value) + ", a_type_name = \"" + a_type_name + "\", a_subtype = (" + ToString(a_subtype, verbose) + ")";
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

  void ObjectValueContainer::nextSubtype() {
    Debug<false> debug = Debug<false>(this);
    debug.functionStart("nextSubtype", false, __FILE__, __LINE__);
    debug.debug("this = " + this->toString(true));
    if (a_value == ObjectValue::ARRAY) {
      assert(a_subtype.size() > 0);
      if (a_subtype.size() == 1) {
      	ObjectValueContainer& x = a_subtype.front();
	a_value = x.a_value;
	a_type_name = x.a_type_name;
      }
      a_subtype.pop_front();
    } else {
      a_value = ObjectValue::NONE;
      a_type_name = "";
    }
    debug.functionEnd("nextSubtype: " + this->toString(true));
  }
  
  bool ObjectValueContainer::numberEquals(ObjectValue l, ObjectValue r) const {
    return
      (l == ObjectValue::NUMBER) &&
      (r == ObjectValue::INTEGER || r == ObjectValue::REAL || r == ObjectValue::NUMBER);
  }

  bool ObjectValueContainer::equals(const Array& l, const Array& r) const {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("equals", false, __FILE__, __LINE__);
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
    debug.functionEnd("equals: " + std::to_string(result));
    return result;
  }

  bool ObjectValueContainer::equals(const ObjectValueContainer& other) const {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("equals(this = [" + toString(true) + "], other = [" + other.toString(true) + "])", false, __FILE__, __LINE__);
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
    debug.functionEnd("equals: " + std::to_string(result));
    return result;
  }

  bool ObjectValueContainer::equalsExact(const ObjectValueContainer& other) const {
    Debug<false> debug = Debug<false>(this);
    debug.functionStart("equalsExact(other = " + other.toString(true) + ")", false, __FILE__, __LINE__);
    debug.debug("this = " + this->toString(true));
    bool match = (a_value == other.a_value);
    debug.debug(ast::toString(a_value) + (match ? " == " : " != ") + ast::toString(other.a_value));
    if (match) {
      std::string s1 = a_type_name; 
      std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
      std::string s2 = other.a_type_name; 
      std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);
      match = (s1 == s2);
      debug.debug(s1 + (match ? " == " : " != ") + s2);
    }
    debug.functionEnd("equalsExact: " + std::string(match ? "true" : "false"));
    return match;
  }

  void ObjectTypes::nextSubtype() {
    for (auto& i: m_types) {
      i.nextSubtype();
    }
  }
  
  bool ObjectTypes::equalsExact(const ObjectValueContainer& x) const {
    for (auto& i: m_types) {
      if (i.equalsExact(x)) { return true; };
    }
    return false;
  }

  bool ObjectTypes::equals(ObjectValue x) const {
    for (auto& i: m_types) {
      if (i.IsValue(x)) {
	return true;
      }
    }
    return false;
  }

  void ObjectTypes::add(const ObjectValueContainer& x) {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("add(x = " + x.toString(true) + ")", false, __FILE__, __LINE__);
    assert(!x.IsValue(ObjectValue::UNKNOWN));
    m_types.push_back(x);
    debug.functionEnd("add");
  }

  bool ObjectTypes::equals(const ObjectValueContainer& x) const {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("equals(this = [" + toString(true) + "], x = [" + x.toString(true) + "])", false, __FILE__, __LINE__);
    bool result = false;
    for (auto& i: m_types) {
      if (i.equals(x)) {
	result = true;
	break;
      }
    }
    debug.functionEnd("equals: " + std::to_string(result));
    return result;
  }

  bool ObjectTypes::equals(ObjectTypes& x) const {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("equals", false, __FILE__, __LINE__);
    bool result = false;
    for (auto& i: x.m_types) {
      if (equals(i)) {
        result = true;
	break;
      }
    }
    debug.functionEnd("equals: " + std::to_string(result));
    return result;
  }

  std::string ObjectTypes::toString(bool verbose) const {
    std::string delimiter;
    std::string result;
    for (auto& i : m_types) {
      result += delimiter + i.toString(verbose);
      delimiter = ", ";
    }
    return result;
  }

  bool ObjectTypes::isArray() {
    Debug<true> debug = Debug<true>(this);
    debug.functionStart("isArray", false, __FILE__, __LINE__);
    debug.debug("this = " + toString());
    assert(m_types.size() == 1);
    ObjectValue x = m_types.front().GetValue();
    bool result = (x == ObjectValue::ARRAY);
    debug.functionEnd("isArray: " + std::to_string(result));
    return result;
  }

  std::string ObjectArgument::toString(bool verbose) const {
    return m_name + (m_type_name.empty() ? " " : " : " + m_type_name) + "(" + m_types.toString(verbose) + ")";
  }
  
  std::string ObjectArguments::toString(bool verbose) const {
    std::string delimiter = "";
    std::string s;
    for (auto& a : m_elements) {
      s += delimiter + a.toString(verbose);
      delimiter = ", ";
    }
    return s;
  }

}

 
