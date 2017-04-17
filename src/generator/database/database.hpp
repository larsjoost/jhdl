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

    bool find(DatabaseResults& objects, std::string& name, std::string package = "", std::string library = "");
    
    void descendHierarchy(std::string& name);
    void ascendHierarchy();

    bool setVisible(std::string& name, std::string package = "", std::string library = "");
 };

  
  
}

#endif
