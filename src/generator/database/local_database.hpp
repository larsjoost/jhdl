#ifndef _LOCAL_DATABASE_HPP_
#define _LOCAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"
#include "name_map.hpp"

namespace generator {

  class LocalDatabase {
    std::string library;
    std::string package;
    std::list<NameMap> map;

    Exceptions exceptions;

    void add(std::string& name, DatabaseElement& e);

  public:
           
    void find(DatabaseResults& results, std::string& name);

    int getHierarchyLevel(std::string& name);
    
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

    void setLibrary(std::string& name);
    std::string getLibrary();
    void setPackage(std::string& name);
    std::string getPackage();
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    std::string getParentName(int hierarchy = 0);
    void print();
  };
  
  
}

#endif
