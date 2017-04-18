#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"
#include "local_database.hpp"
#include "global_database.hpp"

namespace generator {

  class Database {
    LocalDatabase localDatabase;
    GlobalDatabase globalDatabase;
  public:

    void append(std::string& library, Database& database);

    void add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type, ast::ObjectArguments arguments = {});
    void addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::Attribute* attribute);
    void addFunction(std::string& name, ast::ObjectArguments& arguments,
                     ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function);
    void addProcedure(std::string& name, ast::ObjectArguments& arguments, ast::ProcedureDeclaration* procedures);

    bool find(DatabaseResults& objects, std::string& name, std::string package = "", std::string library = "");
    
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    bool setVisible(std::string& name, std::string package = "", std::string library = "");
 };

  
  
}

#endif
