
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
    class IndexOutOfRange {
    };
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
    void set(const ObjectValueContainer& other) {
      a_value = other.a_value;
      a_type_name = other.a_type_name;
      a_arguments = other.a_arguments;
      a_subtype = other.a_subtype;
    }
    bool IsValue(ObjectValue other) const {return a_value == other; }
    bool IsArrayWithDimension(const int dim) {
      return IsValue(ObjectValue::ARRAY) && (a_arguments.size() == 1) && !SubtypeIsValue(ObjectValue::ARRAY);
    }
    bool SubtypeIsValue(ObjectValue other) const {
      ObjectValueContainer x;
      if (a_subtype.size() == 1) {
        return a_subtype.front().IsValue(other);
      }
      return false;
    }
    Array& GetSubtype() { return a_subtype; }
    bool SetSubtype(ObjectValueContainer& other) const {
      bool result = true;
      if (a_subtype.empty()) {
        other = ObjectValueContainer();
        result = false;
      } else {
        assert(a_subtype.size() == 1);
        other.set(a_subtype.front());
      }
      return result;
    }
    Array& GetArguments() { return a_arguments; }
    ObjectValueContainer& GetArgument(const int index = 0) {
      if (a_arguments.size() > index) { 
        auto it = a_arguments.begin();
        std::advance(it, index);
        return *it;
      }
      throw IndexOutOfRange();
    }
    ObjectValue GetValue() const { return a_value; }
    ObjectValue GetArgumentValue(const int index = 0) {
      ObjectValueContainer& o = GetArgument(index);
      return o.GetValue();
    }
    std::string GetTypeName() const { return a_type_name; }
    bool HasArray(ObjectValue value) const {
      return ((value == ObjectValue::ARRAY) || (value == ObjectValue::ACCESS) || (value == ObjectValue::FILE));
    }
    void set(ObjectValue value, const Array& arguments, const ObjectValueContainer& subtype) {
      assert(HasArray(value));
      a_value = value;
      a_subtype.push_back(subtype);
      a_arguments = arguments;
    }
    ObjectValueContainer(ObjectValue value = ObjectValue::UNKNOWN, std::string type_name = "") {
      if (HasArray(value)) {
        a_subtype.push_back(ObjectValueContainer(ObjectValue::UNKNOWN));
      }
      if (value == ObjectValue::BOOLEAN) {
        a_value = ObjectValue::ENUMERATION;
        a_type_name = "BOOLEAN";
      } else {
        a_value = value;
        a_type_name = type_name;
      }
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
    bool equals(ObjectArguments& other, bool array_type = false, bool verbose = false);
    bool equals(ObjectValueContainer::Array& other, bool array_type = false, bool verbose = false);
    bool equals(const ObjectValueContainer& other, bool verbose = false);
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
