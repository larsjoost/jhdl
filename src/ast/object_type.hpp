
#ifndef AST_OBJECT_TYPE_HPP_
#define AST_OBJECT_TYPE_HPP_

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

  class ObjectArguments;
  
  class ObjectValueContainer {
    ObjectValue a_value = ObjectValue::UNKNOWN;
    std::string a_type_name;
  public:
    class IndexOutOfRange {};
    typedef std::list<ObjectValueContainer> Array;
  private:
    Array a_arguments;
    Array a_subtype;
  public:
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
    ObjectValueContainer(const ObjectValueContainer& value) {
      set(value);
    }
    bool numberEquals(ObjectValue l, ObjectValue r) const;
    bool equals(const Array& l, const Array& r) const;
    bool equals(const ObjectValueContainer& other) const;
    bool equals(const ObjectArguments& arguments);
    bool equalsExact(const ObjectValueContainer& other) const;
    inline bool operator==(const ObjectValueContainer& other) const {
      return equals(other);
    }
    inline bool operator!=(const ObjectValueContainer& other) const {
      return !equals(other);
    }
    void set(const ObjectValueContainer& other) {
      a_value = other.a_value;
      a_type_name = other.a_type_name;
      a_arguments = other.a_arguments;
      a_subtype = other.a_subtype;
    }
    bool IsValue(ObjectValue other) const {return a_value == other; }
    bool isArray() { return IsValue(ObjectValue::ARRAY); }
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
    void nextSubtype();
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
    ObjectValueContainer& GetArgument(const unsigned int index = 0) {
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
    void setTypeName(std::string name) { assert(a_type_name.empty()); a_type_name = name; }
    bool HasArray(ObjectValue value) const {
      return ((value == ObjectValue::ARRAY) || (value == ObjectValue::ACCESS) || (value == ObjectValue::FILE));
    }
    void set(ObjectValue value, const Array& arguments, const ObjectValueContainer& subtype) {
      assert(HasArray(value));
      a_value = value;
      a_subtype.push_back(subtype);
      a_arguments = arguments;
    }
    std::string toString(bool verbose = false) const;
    std::string toString(const Array& l, bool verbose = false) const;
  };

  struct ObjectTypes {
    std::list<ObjectValueContainer> m_types;
    ObjectTypes() {}
    ObjectTypes(ObjectValue x) { set(x); }
    ObjectTypes(const ObjectValueContainer& x) { add(x); }
    bool equals(ObjectValue x) const;
    bool equals(ObjectTypes& x) const;
    bool equals(const ObjectValueContainer& x) const;
    bool isUnique() { return m_types.size() == 1; }
    ObjectValueContainer& get() { assert(!m_types.empty()); return m_types.front(); }
    void set(const ObjectValueContainer& x) { add(x); }
    void set(ObjectValue x) { add(x); }
    void set(ObjectTypes& x) { for (auto i : x.m_types) { add(i); }; }
    void add(ObjectValue x) { ObjectValueContainer y(x); add(y); }
    void add(const ObjectValueContainer& x);
    void nextSubtype();
    bool isArray();
    bool equalsExact(const ObjectValueContainer& x) const;
    std::string toString(bool verbose = false) const;
  };
  
  struct ObjectArgument {
    std::string m_name;
    std::string m_type_name;
    ObjectTypes m_types;
    ObjectArgument(const ObjectValueContainer& type) {m_types.set(type);}
    ObjectArgument(std::string name) : m_name(name) { m_types.set(ObjectValue::USER_TYPE); }
    ObjectArgument() {}
    bool equals(const ObjectValueContainer& x) const { return m_types.equals(x); }
    std::string toString(bool verbose = false) const;
  };


  class ObjectArguments {
    std::list<ObjectArgument> m_elements;
  public:
    ObjectArguments(std::list<ObjectArgument> o) {
      m_elements = o;
    };
    ObjectArguments() {}
    void add(ObjectArgument& o) {m_elements.push_back(o);}
    bool empty() { return m_elements.empty(); }
    int size() const { return m_elements.size(); }
    const std::list<ObjectArgument>& getList() const { return m_elements; }
    std::string toString(bool verbose = false) const;
  };

  class Expression;

  struct ObjectInterfaceElement {
    std::string m_name;
    std::string m_type_name;
    ObjectValueContainer m_type;
    Expression* m_default_value = NULL;
    std::string toString(bool verbose = false);
  };
  
  class ObjectInterface {
    std::list<ObjectInterfaceElement> m_elements;
    int match(const ObjectArgument& association, int index);
  public:
    template<class T>
    bool exactMatch(T& other);
    bool matches(ObjectArguments& association);
    std::list<ObjectInterfaceElement>& getList() { return m_elements; }
    bool empty() { return m_elements.empty(); }
    int size() const { return m_elements.size(); }
    void add(ObjectInterfaceElement& e) { m_elements.push_back(e); }
    std::string toString(bool verbose = false);
  };

  template<class T>
  bool ObjectInterface::exactMatch(T& other) {
    Debug<false> debug = Debug<false>(this);
    debug.functionStart("exactMatch");
    bool result = true;
    if (size() == other.size()) {
      auto l = m_elements.begin();
      auto r = other.getList().begin();
      while (l != m_elements.end() && r != other.getList().end()) {
        if (l->m_type_name != r->m_type_name) {
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
    debug.functionEnd("exactMatch");
    return result;
  }

  struct ReturnTypes {
  
    struct ReturnTypesHash {
      std::size_t operator() (const ObjectValueContainer& o) const {
	ObjectValue v = o.GetValue();
	if (v == ObjectValue::NUMBER || v == ObjectValue::INTEGER || v == ObjectValue::REAL) {
	  v = ObjectValue::NUMBER;
	}
	size_t result = static_cast<std::underlying_type_t<ast::ObjectValue>>(v);
	// result += std::hash<std::string>{}(o.GetTypeName());
	return result;
      }
    };

    typedef std::unordered_set<ObjectValueContainer, ReturnTypesHash> List; 
    
    List m_list;

    List& getList() { return m_list; }
    
    void insert(const ObjectValueContainer& x) { m_list.insert(x); }
    void clear() { m_list.clear(); }
    bool empty() { return m_list.empty(); }
    int size() { return m_list.size(); }
    void append(ReturnTypes& x) { m_list.insert(x.getList().begin(), x.getList().end()); };
    const ObjectValueContainer& front() { return *m_list.begin(); }
    std::string toString(bool verbose = false) {
      std::string result;
      std::string delimiter;
      for (auto& i : m_list) {
	result += delimiter + i.toString(verbose);
	delimiter = ", ";
      };
      return result;
    };
  };
  
};

#endif
