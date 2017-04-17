#include "database.hpp"

namespace generator {
  
  void Database::append(std::string& library, Database& database) {
    globalDatabase.append(library, database.localDatabase);
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type, ast::ObjectArguments arguments) {
    localDatabase.add(id, name, type, arguments); 
  }

  bool Database::find(DatabaseResults& objects, std::string& name, std::string package, std::string library) {
    if (package.empty() && library.empty()) {
      localDatabase.find(objects, name);
    }
    globalDatabase.find(objects, name, package, library);
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
  

