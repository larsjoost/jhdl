
#ifndef AST_OBJECT_TYPE_H_
#define AST_OBJECT_TYPE_H_

#include "../debug/debug.hpp"

#include <list>
#include <string>
#include <unordered_set>

namespace ast {

  enum class ObjectType {
    SIGNAL, VARIABLE, CONSTANT, FUNCTION, PROCEDURE, PORT, TYPE,
    ENUM, ENTITY, CONFIGURATION, SUBTYPE, FILE, GROUP, ARCHITECTURE,
    GENERATE, PACKAGE, PACKAGE_BODY, COMPONENT, BLOCK, PROCESS, FACTORY, UNKNOWN};

  std::string toString(ObjectType o);

  enum class ObjectValue {
    INTEGER, REAL, NUMBER, CHARACTER, PHYSICAL, ARRAY, ENUMERATION,
    ACCESS, FILE, BOOLEAN, USER_TYPE, UNKNOWN, NONE, DONT_CARE};

  std::string toString(ObjectValue o);

  class ObjectValueContainer {
    ObjectValue a_value = ObjectValue::UNKNOWN;
    std::string a_type_name;
  public:
    typedef std::list<ObjectValueContainer> Array;
  private:
    Array a_arguments;
    Array a_subtype;
  public:
    bool numberEquals(ObjectValue l, ObjectValue r) const;
    bool equals(const Array& l, const Array& r) const;
    bool equals(const ObjectValueContainer& other) const;
    bool operator==(const ObjectValueContainer& other) const {
      return equals(other);
    }
    bool IsValue(ObjectValue other) {return a_value == other; }
    Array& GetSubtype() { return a_subtype; }
    Array& GetArguments() { return a_arguments; }
    ObjectValue GetValue() const { return a_value; }
    std::string GetTypeName() const { return a_type_name; }
    ObjectValueContainer(ObjectValue value = ObjectValue::UNKNOWN, std::string type_name = "") {
      assert(!HasArray(value));
      if (value == ObjectValue::BOOLEAN) {
        a_value = ObjectValue::ENUMERATION;
        a_type_name = "BOOLEAN";
      } else {
        a_value = value;
        a_type_name = type_name;
      }
    }
    bool HasArray(ObjectValue value) const {
      return ((value == ObjectValue::ARRAY) || (value == ObjectValue::ACCESS) || (value == ObjectValue::FILE));
    }
    void set(ObjectValue value, const Array& arguments, const ObjectValueContainer& subtype) {
      assert(HasArray(value));
      a_value = value;
      a_subtype.push_back(subtype);
      a_arguments = arguments;
    }
    ObjectValueContainer(ObjectValue value, const Array& arguments, const ObjectValueContainer& subtype) {
      set(value, arguments, subtype);
    }
    ObjectValueContainer(ObjectValue value, const ObjectValueContainer& argument, const ObjectValueContainer& subtype) {
      Array arguments;
      arguments.push_back(argument);
      set(value, arguments, subtype);
    }
    ObjectValueContainer(ObjectValue value, const ObjectValueContainer& subtype) {
      Array arguments;
      set(value, arguments, subtype);
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
    std::string toString(bool verbose = false) const;
    std::string ToString(const Array& l, bool verbose = false) const;
  };

  class Expression;
  
  struct ObjectArgument {
    std::string name;
    std::string type_name;
    ObjectValueContainer type;
    Expression* default_value = NULL;
    ObjectArgument(const ObjectValueContainer& type) : type(type) {}
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
    bool equals(ObjectValueContainer::Array& other, bool verbose = false);
    bool ExactMatch(ObjectArguments& other);
    bool empty() { return list.empty(); }
    int size() { return list.size(); }
    std::string toString();
  };  

  struct ReturnTypesHash {
    std::size_t operator() (const ast::ObjectValueContainer& o) const {
      ObjectValue v = o.GetValue();
      if (v == ObjectValue::NUMBER || v == ObjectValue::INTEGER || v == ObjectValue::REAL) {
        v = ObjectValue::NUMBER;
      }
      size_t result = static_cast<std::underlying_type_t<ast::ObjectValue>>(v);
      // result += std::hash<std::string>{}(o.GetTypeName());
      return result;
    }
  };

  typedef std::unordered_set<ast::ObjectValueContainer, ReturnTypesHash> ReturnTypes;
  
}

#endif
