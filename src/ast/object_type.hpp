
#ifndef AST_OBJECT_TYPE_H_
#define AST_OBJECT_TYPE_H_

#include <list>
#include <string>

namespace ast {

  enum ObjectType {
    SIGNAL, VARIABLE, CONSTANT, FUNCTION, PROCEDURE, PORT, TYPE,
    ENUM, ENTITY, CONFIGURATION, SUBTYPE, FILE, GROUP, ARCHITECTURE,
    PACKAGE, COMPONENT, NUMBER_OF_OBJECT_TYPES};

  std::string toString(ObjectType o);

  enum ObjectValue {
    INTEGER, REAL, BOOLEAN, CHARACTER, TEXT,  PHYSICAL, ARRAY, USER_TYPE, UNKNOWN, NONE, NUMBER_OF_OBJECT_VALUES};

  std::string toString(ObjectValue o);

  struct ObjectValueContainer {
    ObjectValue value;
    std::string typeName;
    bool equal(ObjectValueContainer& other);
    ObjectValueContainer(ObjectValue value = UNKNOWN, std::string typeName = "") : value(value), typeName(typeName) {}
  };
  
  std::string toString(ObjectValueContainer& o);

  struct ObjectArgument {
    std::string name = "";
    ObjectValueContainer type;
    bool defaultValue = false;
    ObjectArgument(ObjectValueContainer& type) : type(type) {}
    ObjectArgument() {}
  };

  std::string toString(ObjectArgument& a);
  
  using ObjectArguments = std::list<ObjectArgument>;

  std::string toString(ObjectArguments& a);

  bool match(ObjectArguments& l, ObjectArguments& r);
  
}

#endif
