#ifndef _LOCAL_DATABASE_HPP_
#define _LOCAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../../debug/debug.hpp"
#include "general.hpp"
#include "name_map.hpp"

namespace generator {

  class LocalDatabase {
    std::string a_library;
    ast::ObjectType a_type;
    std::string a_name;
    std::list<ast::Attribute*> a_package_attributes;
    std::list<NameMap> a_map;

    bool a_verbose = false;
    
    Exceptions a_exceptions;

    void add(std::string& name, DatabaseElement& e);

    void printAllObjects(std::string& name);

  public:
           
    void find(DatabaseResults& results, std::string& name, bool local = true);

    int getHierarchyLevel();
    
    bool setVisible(std::string name = "");

    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
             ast::ObjectArguments arguments = ast::ObjectArguments(false),
             ast::Text* text = NULL);
    void addAttribute(std::string& name, ast::ObjectArguments& arguments,
                      ast::ObjectType id, ast::Attribute* attribute,
                      ast::Text* text = NULL);
    void addFunction(std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType,
                     ast::FunctionDeclaration* function,
                     ast::Text* text = NULL);
    void addProcedure(std::string& name, ast::ObjectArguments& arguments,
                      ast::ProcedureDeclaration* procedures,
                      ast::Text* text = NULL);

    void add(LocalDatabase* other);
    
    void initialize(std::string& library, std::string& name, ast::ObjectType type);
    std::string getLibrary();
    std::string getName();
    ast::ObjectType getType();
    
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    std::string getParentName();
    void print();
  };
  
}

#endif
