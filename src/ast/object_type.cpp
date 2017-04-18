#include <string>
#include "object_type.hpp"

namespace ast {

  std::string toString(ObjectType o) {
    static std::string a[NUMBER_OF_OBJECT_TYPES] = {
      "SIGNAL", "VARIABLE", "CONSTANT", "FUNCTION", "PROCEDURE", "PORT", "TYPE",
      "ENUM", "ENTITY", "CONFIGURATION", "SUBTYPE", "FILE", "GROUP", "ARCHITECTURE",
      "PACKAGE", "COMPONENT"};
    return a[o];
  }
  
  std::string toString(ObjectValue o) {
    static std::string a[NUMBER_OF_OBJECT_VALUES] = {
      "INTEGER", "REAL", "BOOLEAN", "CHARACTER", "TEXT", "PHYSICAL", "ARRAY", "USER_TYPE", "UNKNOWN", "NONE"};
    return a[o];
  }

  std::string toString(ObjectValueContainer& o) {
    if (o.value == USER_TYPE) {
      return o.typeName;
    }
    return toString(o.value);
  }

  int ObjectArguments::match(ObjectArguments& interface, ObjectArgument& association, int index) {
    auto it = interface.list.begin();
    if (association.name.empty() && index >= 0 && interface.list.size() > index) {
      std::advance(it, index);
      return it->type.equal(association.type) ? index : -1;
    } else {
      for (int i = 0; i < interface.list.size(); i++) {
        if (it->name == association.name) {
          return it->type.equal(association.type) ? i : -1;
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
    int size = interface.list.size();
    bool m[size] {};
    int index = 0;
    for (ObjectArgument& a : association.list) {
      int i = match(interface, a, index);
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
  
}
