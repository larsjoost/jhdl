#include <cassert>
#include "database.hpp"

namespace generator {

  std::string Database::globalName(DatabaseResult& object, std::string name) {
    if (object.object->sectionName != localDatabase.getName() ||
        object.object->sectionType != localDatabase.getType()) {
      std::string separator1 = "::";
      std::string separator2 = "::";
      if (object.object->id == ast::FUNCTION || object.object->id == ast::PROCEDURE) {
	separator1 = "_";
	separator2 = ".";
      }
      name = object.object->library + separator1 + object.object->sectionName + separator2 + name;
    }
    if (object.object->id == ast::TYPE) {
      name = name + "<>";
    }
    return name;
  }
  
  bool Database::globalName(std::string& name, ast::ObjectType id) {
    bool found = false;
    auto valid = [&](DatabaseElement* e) {
      return e->id == id;
    };
    DatabaseResult object;
    if (findOne(object, name, valid)) {
      found = true;
      name = globalName(object, name);
    } else {
      exceptions.printError("Unable to find " + ast::toString(id) + " " + name);
    }
    return found;
  }

  std::string Database::globalName(std::string name) {
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    DatabaseResult object;
    if (findOne(object, name, valid)) {
      name = globalName(object, name);
    } else {
      exceptions.printError("Unable to find " + name);
    }
    return name;
  }

  ast::ObjectValueContainer Database::getType(std::string name, std::string package, std::string library) {
    auto valid = [&] (DatabaseElement* e) { return e->id == ast::TYPE; };
    ast::ObjectValueContainer result(ast::UNKNOWN);
    DatabaseResult object;
    if (findOne(object, name, valid, package, library)) {
      result = object.object->type;
    } else {
      std::string msg = "Unable to find type " + name + " in package " + library + "." + package;
      throw ObjectNotFoundException(msg);
    }
    return result;
  }

  std::string Database::getLibrary() {
    return localDatabase.getLibrary();
  }
  
  std::string Database::getName() {
    return localDatabase.getName();
  }

  ast::ObjectType Database::getType() {
    return localDatabase.getType();
  }

  void Database::Globalize() {
    assert(localDatabase.getHierarchyLevel() == 0);
    globalDatabase.append(localDatabase);
  }

  std::string Database::getParentName() {
    return localDatabase.getParentName();
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

  void Database::topHierarchyStart(std::string& library, std::string& name, ast::ObjectType type) {
    localDatabase.initialize(library, name, type);
    descendHierarchy(name);
  }
  
  void Database::topHierarchyEnd(bool globalize) {
    if (globalize) {
      Globalize();
    }
    ascendHierarchy();
  }

  void Database::descendHierarchy(std::string& name) {
    localDatabase.descendHierarchy(name);
  }
  void Database::ascendHierarchy() {
    localDatabase.ascendHierarchy();
  }

  int Database::getHierarchyLevel() {
    return localDatabase.getHierarchyLevel();
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

  void Database::printAllObjects(std::string name) {
    auto valid = [&](DatabaseElement* e) { return true; };
    DatabaseResults objects;
    findAll(objects, name, valid);
    if (!objects.empty()) {
      std::cerr << "Found the following objects with name \"" + name + "\":" << std::endl;
      for (auto& i : objects) {
	std::cerr << i.toString() << std::endl;
      }
    } else {
      std::cerr << "Found no objects with name \"" + name + "\"." << std::endl;
    }
  }
  
}
  

