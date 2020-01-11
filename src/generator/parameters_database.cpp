#include <cassert>

#include "parameters.hpp"
#include "name_converter.hpp"

namespace generator {

  bool parameters::findOne(DatabaseResult& object, std::string& name, ast::ObjectType type, std::string package, std::string library) {
    debug.functionStart("findOne");
    auto valid =
      [&](DatabaseElement* e) {
	return e->id == type;
      };
    bool found = findOneGeneric(object, name, valid, package, library);
    debug.functionEnd("findOne");
    return found;
  }
  
  bool parameters::findOne(DatabaseResult& object, std::string& name, std::string package, std::string library) {
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    return findOneGeneric(object, name, valid, package, library);
  }
  
  bool parameters::findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier, ast::ObjectType type) {
    assert(identifier);
    std::string name = identifier->toString(true);
    if (!findOne(result, name, type)) {
      exceptions.printError("Could not find declaration of " + ast::toString(type) + "\"" + name + "\"", &identifier->text);
      if (a_verbose) {
	printDatabase();
      }
      return false;
    }
    return true;
  }
  
  bool parameters::findOne(DatabaseResult& result, ast::SimpleIdentifier* identifier) {
    assert(identifier);
    std::string name = identifier->toString(true);
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    if (!findOneGeneric(result, name, valid)) {
      exceptions.printError("Could not find declaration of \"" + name + "\"", &identifier->text);
      if (a_verbose) {
	printDatabase();
      }
      return false;
    }
    return true;
  }

  void parameters::findAll(DatabaseResults& objects, const std::string& name, std::string package, std::string library) {
    getActiveClassContainer()->database.findAll(objects, name, package, library);
  }
 
  
  void parameters::printAllObjects(std::string name) {
    getActiveClassContainer()->database.printAllObjects(name);
  }

  std::string parameters::getName(ast::SimpleIdentifier* i, ast::ObjectType o) {
    DatabaseResult result;
    if (!findOne(result, i, o)) {
      assert(false);
    }
    return getName(result);
  }

  std::string parameters::getName(DatabaseResult& object) {
    return NameConverter::getName(*this, object);
  }
    
  bool parameters::getParent(ParentInfo& p) {
    ClassContainer* c = getParentClassContainer();
    if (c != NULL) {
      p.name = c->name;
      p.type = c->type;
    }
    return c != NULL;
  }

  void parameters::addAttribute(std::string& name, ast::ObjectArguments& arguments,
				ast::ObjectType id, ast::Attribute* attribute,
				ast::Text* text) {
    getActiveClassContainer()->database.addAttribute(name, arguments,
						     id, attribute, text);

  }
  void parameters::addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
			       ast::ObjectValueContainer returnType, ast::FunctionDeclaration* function,
			       ast::Text* text) {
    getActiveClassContainer()->database.addFunction(type, name, arguments,
						    returnType, function,
						    text);
  }
  
  void parameters::addObject(ast::ObjectType id, std::string& name,
			     ast::ObjectValueContainer type,
			     ast::ObjectArguments arguments,
			     ast::Text* text) {
    debug.functionStart("addObject");
    getActiveClassContainer()->database.add(id, name,  type,
					    arguments, text);
    debug.functionEnd("addObject");
  }
  
  void parameters::addObject(ast::ObjectType id, std::string& name,
			     ast::ObjectValue type,
			     ast::Text* text) {
    debug.functionStart("addObject");
    getActiveClassContainer()->database.add(id, name,  type,
					    text);
    debug.functionEnd("addObject");
  }

  bool parameters::exists(std::string& library, std::string& package) {
    return a_global_database->exists(library, package);
  }

  bool parameters::exists(std::string& library, std::string& name, ast::ObjectType type) {
    DatabaseResult x;
    return findOne(x, name, type, "", library);
  }
    
  bool parameters::setVisible(std::string& name, std::string package, std::string library) {
    return a_global_database->setVisible(name, package, library);
  }

  void parameters::printDatabase(std::string name) {
    ClassContainer* c = getCurrentClassContainer();
    if (c) {c->database.print(name);}
    a_global_database->print(name);
  }

  void parameters::globalizeClass() {
    debug.functionStart("globalizeClass");
    assert(!file_container.content.children.empty());
    ClassContainer& current_top_class = file_container.content.children.back();
    a_global_database->append(current_top_class.database.local_database, file_container.library, current_top_class.name);
    debug.functionEnd("globalizeClass");
  }
  
}
