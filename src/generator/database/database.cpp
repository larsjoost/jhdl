#include "database.hpp"

namespace generator {
  
  void Database::append(std::string& library, Database& database) {
    globalDatabase.append(library, database.localDatabase);
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type, ast::ObjectArguments arguments) {
    localDatabase.add(id, name, type, arguments); 
  }

  void Database::descendHierarchy(std::string& name) {
    localDatabase.descendHierarchy(name);
  }
  void Database::ascendHierarchy() {
    localDatabase.ascendHierarchy();
  }

  bool Database::setVisible(std::string& name, std::string package, std::string library) {
    globalDatabase.setVisible(name, package, library);
  }
  
}
  

