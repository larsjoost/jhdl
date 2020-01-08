#ifndef _LOCAL_DATABASE_HPP_
#define _LOCAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../../debug/debug.hpp"
#include "general.hpp"
#include "name_map.hpp"

namespace generator {

  class LocalDatabase {
    NameMap a_content;

    bool a_verbose = false;
    
    Exceptions a_exceptions;

    void add(std::string& name, DatabaseElement& e);

    void printAllObjects(std::string& name);

  public:
           
    void find(DatabaseResults& results, const std::string& name);

    bool setVisible(std::string name = "");

    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
             ast::ObjectArguments arguments = ast::ObjectArguments(false),
             ast::Text* text = NULL);
    void addAttribute(std::string& name, ast::ObjectArguments& arguments,
                      ast::ObjectType id, ast::Attribute* attribute,
                      ast::Text* text = NULL);
    void addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType,
                     ast::FunctionDeclaration* function,
                     ast::Text* text = NULL);

    std::string getLibrary();
    std::string getName();
    ast::ObjectType getType();
    
    void print();
  };
  
}

#endif
