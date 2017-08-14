
#ifndef AST_OBJECT_TYPE_H_
#define AST_OBJECT_TYPE_H_

#include <list>
#include <string>

#include "../debug/debug.hpp"

namespace ast {

  enum class ObjectType {
    SIGNAL, VARIABLE, CONSTANT, FUNCTION, PROCEDURE, PORT, TYPE,
    ENUM, ENTITY, CONFIGURATION, SUBTYPE, FILE, GROUP, ARCHITECTURE,
    PACKAGE, PACKAGE_BODY, COMPONENT};

  std::string toString(ObjectType o);

  enum class ObjectValue {
    INTEGER, REAL, NUMBER, CHARACTER, TEXT,  PHYSICAL, ARRAY, ENUMERATION,
    ACCESS, FILE, BOOLEAN, USER_TYPE, UNKNOWN, NONE, DONT_CARE};

  std::string toString(ObjectValue o);

  class ObjectValueContainer {
    ObjectValue a_value = ObjectValue::UNKNOWN;
    std::string a_type_name;
    ObjectValueContainer* a_subtype = NULL;
  public:
    bool numberEquals(ObjectValue l, ObjectValue r);
    bool equals(ObjectValueContainer& other);
    bool IsValue(ObjectValue other) {return a_value == other; }
    ObjectValueContainer* GetSubtype() { return a_subtype; }
    ObjectValue GetValue() { return a_value; }
    std::string GetTypeName() { return a_type_name; }
    ObjectValueContainer(ObjectValue value = ObjectValue::UNKNOWN, std::string type_name = "") {
      assert(!HasSubtype(value));
      if (value == ObjectValue::BOOLEAN) {
        a_value = ObjectValue::ENUMERATION;
        a_type_name = "BOOLEAN";
      } else {
        a_value = value;
        a_type_name = type_name;
      }
    }
    bool HasSubtype(ObjectValue value) {
      return ((value == ObjectValue::ARRAY) || (value == ObjectValue::ACCESS) || (value == ObjectValue::FILE));
    }
    void set(ObjectValue value, ObjectValueContainer* subtype) {
      assert(HasSubtype(value));
      a_value = value;
      a_subtype = new ObjectValueContainer();
      *a_subtype = *subtype;
    }
    ObjectValueContainer(ObjectValue value, ObjectValueContainer& subtype) {
      set(value, &subtype);
    }
    ObjectValueContainer(ObjectValue value, ObjectValueContainer* subtype) {
      set(value, subtype);
    }
    ObjectValueContainer(std::string type_name) {
      a_value = ObjectValue::USER_TYPE;
      a_type_name = type_name;
    }
    /*
    ~ObjectValueContainer() {
      if (a_subtype) {
        delete a_subtype;
        a_subtype = NULL;
      }
    }
    */
    std::string toString();
  };

  class Expression;
  
  struct ObjectArgument {
    std::string name;
    std::string type_name;
    ObjectValueContainer type;
    Expression* default_value = NULL;
    ObjectArgument(ObjectValueContainer& type) : type(type) {}
    ObjectArgument(std::string name) : name(name) { type = ObjectValue::USER_TYPE; }
    ObjectArgument() { type = ObjectValue::UNKNOWN; }
    std::string toString();
  };


  class ObjectArguments {
    bool isInterface;
    void setDefaultValues(bool m[], ObjectArguments& interface);
    int match(ObjectArguments& interface, ObjectArgument& association, int index, bool verbose);
  public:
    std::list<ObjectArgument> list;
    ObjectArguments(bool isInterface, std::list<ObjectArgument> o = {}) : list(o), isInterface(isInterface) {};
    void push_back(ObjectArgument& o) {list.push_back(o);}
    bool equals(ObjectArguments& other, bool verbose = false);
    bool ExactMatch(ObjectArguments& other);
    bool empty() { return list.empty(); }
    int size() { return list.size(); }
    std::string toString();
  };  

}

#endif
