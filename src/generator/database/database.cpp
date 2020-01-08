#include <cassert>
#include "database.hpp"

namespace generator {

  ast::ObjectValueContainer Database::getType(std::string name, std::string package, std::string library) {
    auto valid = [&] (DatabaseElement* e) { return e->id == ast::ObjectType::TYPE; };
    ast::ObjectValueContainer result(ast::ObjectValue::UNKNOWN);
    DatabaseResult object;
    if (findOne(object, name, valid, package, library)) {
      result = object.object->type;
    } else {
      std::string msg = "Unable to find type " + name + " in package " + library + "." + package;
      throw ObjectNotFoundException(msg);
    }
    return result;
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                     ast::ObjectArguments arguments, ast::Text* text) {
    debug.functionStart("add(id = " + ast::toString(id) + ", name = " + name + ", type = " + type.toString() + ")", true);
    local_database.get()->add(id, name, type, arguments, text);
    debug.functionEnd("add");
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValue type, ast::Text* text) {
    debug.functionStart("add(id = " + ast::toString(id) + ", name = " + name + ", type = " + ast::toString(type) + ")", true);
    ast::ObjectValueContainer t = ast::ObjectValueContainer(type);
    local_database.get()->add(id, name, type, text); 
    debug.functionEnd("add");
  }

  void Database::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id,
                              ast::Attribute* attribute, ast::Text* text) {
    local_database.get()->addAttribute(name, arguments, id, attribute, text);
  }
  void Database::addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                             ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                             ast::Text* text) {
    local_database.get()->addFunction(type, name, arguments, returnType, function, text);
  }

  bool Database::findOne(DatabaseResult& object, std::string& name, ast::ObjectType type,
                         std::string package, std::string library) {
    auto valid = [&](DatabaseElement* e) {
      return e->id == type;
    };
    return findOne(object, name, valid, package, library);
  }

  bool Database::findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier, ast::ObjectType type) {
    assert(identifier);
    std::string name = identifier->toString(true);
    if (!findOne(result, name, type)) {
      exceptions.printError("Could not find declaration of " + ast::toString(type) + "\"" + name + "\"", &identifier->text);
      return false;
    }
    return true;
  }
    
  bool Database::findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier) {
    assert(identifier);
    std::string name = identifier->toString(true);
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    if (!findOne(result, name, valid)) {
      exceptions.printError("Could not find declaration of \"" + name + "\"", &identifier->text);
      return false;
    }
    return true;
  }

  bool Database::findOne(DatabaseResult& object, std::string& name, 
                         std::string package, std::string library) {
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    return findOne(object, name, valid, package, library);
  }

  void Database::findAll(DatabaseResults& objects, const std::string& name, std::string package, std::string library) {
    if (package.empty() && library.empty()) {
      local_database.get()->find(objects, name);
    }
  }

  void Database::print(std::string name) {
    PrintLocal();
  }

  void Database::PrintLocal() {
    std::cout << "Local Database:" << std::endl;
    local_database.get()->print();
  }
  
  void Database::printAllObjects(std::string name) {
    auto valid = [&](DatabaseElement* e) { return true; };
    DatabaseResults objects;
    findAll(objects, name);
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
  

