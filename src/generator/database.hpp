#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../ast/object_type.hpp"
#include "../ast/attribute.hpp"
#include "../ast/function_declaration.hpp"
#include "../ast/procedure_declaration.hpp"

namespace generator {

  struct DatabaseElement {
    std::string section;
    std::string name;
    std::string arguments;
    ast::ObjectType id;
    bool visible;
    ast::Attribute* attribute;
    ast::FunctionDeclaration* function;
    ast::ProcedureDeclaration* procedure;
  };
  
  class ArgumentMap {
    std::unordered_map<std::string, DatabaseElement> map;
  public:
    void setAllVisible();
    void addObject(std::string& arguments, DatabaseElement& e);
    DatabaseElement* findObject(std::string& argument, ast::ObjectType id);
    void print();
    static void print(DatabaseElement* e);
  };
  
  class NameMap {
    std::string section;
    std::unordered_map<std::string, ArgumentMap> map;
  public:
    explicit NameMap(std::string& section) : section(section) { }
    void setAllVisible();
    void setVisible(std::string& name);
    void addObject(std::string& name, std::string& arguments, DatabaseElement& e);
    DatabaseElement* findObject(std::string& name, std::string& argument, ast::ObjectType id);
    std::string getSection() {return section;};
    void print();
  };
  
  class LocalDatabase {

    std::string section;
    std::list<NameMap> map;
  
  public:

    void addObject(std::string& name, ast::ObjectType id);
    void addObject(std::string& name, std::string& arguments, ast::ObjectType id);
    DatabaseElement* findObject(std::string& name, std::string& arguments, ast::ObjectType id);
    DatabaseElement* findObject(std::string& name, int& hierarchyLevel);
    DatabaseElement* findObject(std::string& name);

    int getHierarchyLevel(std::string& name);
    
    void setVisible(std::string& name);
    void setAllVisible();

    void add(std::string& name, std::string& arguments, DatabaseElement& e);
    void addAttribute(std::string& name, std::string& arguments,
                      ast::ObjectType id, ast::Attribute* attribute);
    void addFunction(std::string& name, std::string& arguments,
                     ast::FunctionDeclaration* function);
    void addProcedure(std::string& name, std::string& arguments,
                      ast::ProcedureDeclaration* procedures);
    
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    std::string getParentName(int hierarchy = 0);
    void print();
  };

  class GlobalDatabase {
    std::unordered_map<std::string, LocalDatabase> map;
  public:
    void add(std::string& name, LocalDatabase& d);
    DatabaseElement* findObject(std::string& name, std::string& arguments, ast::ObjectType id, std::string& location);
    DatabaseElement* findObject(std::string& name, std::string& arguments, std::string& location);
    bool setAllVisible(std::string& name);
    bool setVisible(std::string& name, std::string& subname);
    bool exists(std::string& name);
    void print(std::string name);
  };
  
}

#endif
