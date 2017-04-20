#include "database.hpp"

namespace generator {
  
  void Database::globalize(std::string& library) {
    globalDatabase.append(library, localDatabase);
  }

  std::string Database::getParentName(int hierarchy) {
    return localDatabase.getParentName(hierarchy);
  }
  
  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type, ast::ObjectArguments arguments) {
    localDatabase.add(id, name, type, arguments); 
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValue type) {
    ast::ObjectValueContainer t = ast::ObjectValueContainer(type);
    localDatabase.add(id, name, type); 
  }

  void Database::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id, ast::Attribute* attribute) {
    localDatabase.addAttribute(name, arguments, id, attribute);
  }
  void Database::addFunction(std::string& name, ast::ObjectArguments& arguments,
                             ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function) {
    localDatabase.addFunction(name, arguments, returnType, function);
  }

  void Database::addProcedure(std::string& name, ast::ObjectArguments& arguments, ast::ProcedureDeclaration* procedures) {
    localDatabase.addProcedure(name, arguments, procedures);
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

  bool Database::exists(std::string& name) {
    return globalDatabase.exists(name);
  }

}
  

