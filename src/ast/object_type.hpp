
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

  struct ObjectValueContainer {
    ObjectValue value;
    std::string typeName;
    ObjectValueContainer* subtype = NULL;
    bool numberEquals(ObjectValue l, ObjectValue r);
    bool equals(ObjectValueContainer& other);
    ObjectValueContainer(ObjectValue value = ObjectValue::UNKNOWN,
                         std::string typeName = "") {
      if (value == ObjectValue::BOOLEAN) {
        this->value = ObjectValue::ENUMERATION;
        this->typeName = "BOOLEAN";
      } else {
        this->value = value;
        this->typeName = typeName;
      }
    }
    void setSubtype(ObjectValueContainer& subtype) {
      this->subtype = new ObjectValueContainer();
      *this->subtype = subtype;
    };
    ObjectValueContainer(std::string typeName) : typeName(typeName) {
      value = ObjectValue::USER_TYPE;
    }
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
