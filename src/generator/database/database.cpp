#include <cassert>
#include "database.hpp"

namespace generator {

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                     ast::ObjectInterface interface, ast::Text* text) {
    m_debug.functionStart("add(id = " + ast::toString(id) + ", name = " + name + ", type = " + type.toString() + ")", true);
    local_database.get()->add(id, name, type, interface, text);
    m_debug.functionEnd("add");
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValue type, ast::Text* text) {
    m_debug.functionStart("add(id = " + ast::toString(id) + ", name = " + name + ", type = " + ast::toString(type) + ")", true);
    ast::ObjectValueContainer t(type);
    local_database.get()->add(id, name, t, text); 
    m_debug.functionEnd("add");
  }

  void Database::addAttribute(std::string& name, ast::ObjectInterface& interface, ast::ObjectType id,
                              ast::Attribute* attribute, ast::Text* text) {
    local_database.get()->addAttribute(name, interface, id, attribute, text);
  }
  void Database::addFunction(ast::ObjectType type, std::string& name, ast::ObjectInterface& interface,
                             ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                             ast::Text* text) {
    local_database.get()->addFunction(type, name, interface, returnType, function, text);
  }


  void Database::print(std::string name) {
    PrintLocal();
  }

  void Database::PrintLocal() {
    std::cout << "Local Database:" << std::endl;
    local_database.get()->print();
  }
  
  void Database::printAllObjects(std::string name) {
    DatabaseResults objects;
    local_database.get()->findAll(objects, name);
    if (!objects.empty()) {
      std::cerr << "Found the following objects with name \"" + name + "\":" << std::endl;
      for (auto& i : objects) {
	std::cerr << i.toString() << std::endl;
      }
    } else {
      std::cerr << "Found no objects with name \"" + name + "\"." << std::endl;
    }
  }

  void Database::printAll() {
    local_database.get()->print();
  }
  
}
  

