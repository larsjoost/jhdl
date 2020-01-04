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

  bool Database::localize(std::string& library, std::string& name, ast::ObjectType type) {
    LocalDatabase* l = globalDatabase.find(library, name);
    if (l) {
      if (l->getType() == type) {
        localDatabase.add(l);
        return true;
      }
      if (verbose) {
        std::cout << "Expected type = " << ast::toString(type) << ", but found = " << ast::toString(l->getType()) << std::endl;
      }
      return false;
    }
      if (verbose) {
        std::cout << "Did not find " + ast::toString(type) + " " + library + "." + name << std::endl;
      }
    return false;
  }
  
  void Database::GetParent(ParentInfo& parent_info) {
    localDatabase.GetParent(parent_info);
  }
  
  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                     ast::ObjectArguments arguments, ast::Text* text) {
    debug.functionStart("add(id = " + ast::toString(id) + ", name = " + name + ", type = " + type.toString() + ")", true);
    localDatabase.add(id, name, type, arguments, text);
    debug.functionEnd("add");
  }

  void Database::add(ast::ObjectType id, std::string& name, ast::ObjectValue type, ast::Text* text) {
    debug.functionStart("add(id = " + ast::toString(id) + ", name = " + name + ", type = " + ast::toString(type) + ")", true);
    ast::ObjectValueContainer t = ast::ObjectValueContainer(type);
    localDatabase.add(id, name, type, text); 
    debug.functionEnd("add");
  }

  void Database::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id,
                              ast::Attribute* attribute, ast::Text* text) {
    localDatabase.addAttribute(name, arguments, id, attribute, text);
  }
  void Database::addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                             ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
                             ast::Text* text) {
    localDatabase.addFunction(type, name, arguments, returnType, function, text);
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
    if ((package.empty() || package == localDatabase.getName()) &&
        (library.empty() || library == localDatabase.getLibrary())) {
      localDatabase.find(objects, name);
    }
    globalDatabase.find(objects, name, package, library);
  }

  void Database::topHierarchyStart(std::string& library, std::string& name, ast::ObjectType type) {
    localDatabase.initialize(library, name, type);
    openHierarchy(name, type);
  }
  
  void Database::topHierarchyEnd(bool globalize) {
    if (globalize) {
      Globalize();
    }
    closeHierarchy();
  }

  void Database::openHierarchy(const std::string& name, ast::ObjectType type) {
    localDatabase.openHierarchy(name, type);
  }
  void Database::closeHierarchy() {
    localDatabase.closeHierarchy();
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
    PrintLocal();
    std::cout << "Global Database:" << std::endl;
    globalDatabase.print(name);
  }

  void Database::PrintLocal() {
    std::cout << "Local Database:" << std::endl;
    localDatabase.print();
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
    globalDatabase.print();
    localDatabase.print();
  }
  
}
  

