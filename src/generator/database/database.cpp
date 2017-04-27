#include "database.hpp"

namespace generator {
  
  void Database::setLibrary(std::string& name) {
    localDatabase.setLibrary(name);
  }

  void Database::setPackage(std::string& name) {
    localDatabase.setPackage(name);
  }

  void Database::globalize() {
    globalDatabase.append(localDatabase);
  }

  std::string Database::getParentName(int hierarchy) {
    return localDatabase.getParentName(hierarchy);
  }
  
  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                     ast::ObjectArguments arguments, ast::Text* text) {
    localDatabase.add(id, name, type, arguments, text); 
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValue type, ast::Text* text) {
    ast::ObjectValueContainer t = ast::ObjectValueContainer(type);
    localDatabase.add(id, name, type, text); 
  }

  void Database::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id,
                              ast::Attribute* attribute, ast::Text* text) {
    localDatabase.addAttribute(name, arguments, id, attribute, text);
  }
  void Database::addFunction(std::string& name, ast::ObjectArguments& arguments,
                             ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                             ast::Text* text) {
    localDatabase.addFunction(name, arguments, returnType, function, text);
  }

  void Database::addProcedure(std::string& name, ast::ObjectArguments& arguments,
                              ast::ProcedureDeclaration* procedures, ast::Text* text) {
    localDatabase.addProcedure(name, arguments, procedures, text);
  }

  void Database::descendHierarchy(std::string& name) {
    localDatabase.descendHierarchy(name);
  }
  void Database::ascendHierarchy() {
    localDatabase.ascendHierarchy();
  }

  bool Database::setVisible(std::string& name, std::string package, std::string library) {
    std::string n = (name == "ALL") ? "" : name;
    return globalDatabase.setVisible(n, package, library);
  }

  bool Database::exists(std::string& library, std::string& package) {
    return globalDatabase.exists(library, package);
  }

  void Database::print(std::string name) {
    std::cout << "Local Database:" << std::endl;
    localDatabase.print();
    std::cout << "Global Database:" << std::endl;
    globalDatabase.print(name);
  }
  
}
  

