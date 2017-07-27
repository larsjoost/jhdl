
#ifndef AST_OBJECT_TYPE_H_
#define AST_OBJECT_TYPE_H_

#include <list>
#include <string>

namespace ast {

  enum ObjectType {
    SIGNAL, VARIABLE, CONSTANT, FUNCTION, PROCEDURE, PORT, TYPE,
    ENUM, ENTITY, CONFIGURATION, SUBTYPE, FILE, GROUP, ARCHITECTURE,
    PACKAGE, PACKAGE_BODY, COMPONENT, NUMBER_OF_OBJECT_TYPES};

  std::string toString(ObjectType o);

  enum ObjectValue {
    INTEGER, REAL, NUMBER, CHARACTER, TEXT,  PHYSICAL, ARRAY, ENUMERATION,
    BOOLEAN, USER_TYPE, UNKNOWN, NONE, DONT_CARE, NUMBER_OF_OBJECT_VALUES};

  std::string toString(ObjectValue o);

  struct ObjectValueContainer {
    ObjectValue value;
    std::string typeName;
    ObjectValueContainer* subtype;
    bool equals(ObjectValueContainer& other);
    ObjectValueContainer(ObjectValue value = UNKNOWN, std::string typeName = "") {
      if (value == BOOLEAN) {
        this->value = ENUMERATION;
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
    ObjectValueContainer(std::string typeName) : typeName(typeName) { value = USER_TYPE; }
    std::string toString();
  };
  
  struct ObjectArgument {
    std::string name = "";
    ObjectValueContainer type;
    std::string defaultValue = "";
    ObjectArgument(ObjectValueContainer& type) : type(type) {}
    ObjectArgument(std::string name) : name(name) { type = USER_TYPE; }
    ObjectArgument() { type = UNKNOWN; }
    std::string toString();
  };


  class ObjectArguments {
    bool isInterface;
    int match(ObjectArguments& interface, ObjectArgument& association, int index);
  public:
    std::list<ObjectArgument> list;
    ObjectArguments(bool isInterface, std::list<ObjectArgument> o = {}) : list(o), isInterface(isInterface) {};
    void push_back(ObjectArgument& o) {list.push_back(o);}
    bool equals(ObjectArguments& other);
    bool empty() { return list.empty(); }
    std::string toString();
  };  

}

#endif
