#include <cassert>

#include "parameters.hpp"
#include "name_converter.hpp"

namespace generator {

  void parameters::findAllLocal(DatabaseResults& objects, std::string& name, std::string& package, std::string& library) {
    debug.functionStart("findAllLocal(name = " + name + ")");
    std::list<std::string> current_hierarchy = {file_container.library};
    std::shared_ptr<std::list<std::string>> hierarchy_ptr;
    auto action_callback =
      [&](DatabaseResult& r) {
	r.hierarchy = hierarchy_ptr;
	debug.debug("Match = " + r.toString());
      };
    auto class_container_callback =
      [&](ClassContainer& class_container, int hierarchy) {
	current_hierarchy.push_back(class_container.name);
	debug.debug("Hierarchy name = " + class_container.name);
	hierarchy_ptr = std::make_shared<std::list<std::string>>();
	*hierarchy_ptr = current_hierarchy;
	class_container.database.findAll(objects, name, action_callback);
      };
    file_container.traverseClassContainerHierarchy(class_container_callback);
    debug.functionEnd("findAllLocal: " + std::to_string(objects.size()));
  }

  bool parameters::findOne(DatabaseResult& object, std::string& name, ast::ObjectType type, std::string package, std::string library) {
    debug.functionStart("findOne");
    auto valid =
      [&](DatabaseElement* e) {
	return e->id == type;
      };
    bool found = findOneBase(object, name, valid, package, library);
    debug.functionEnd("findOne");
    return found;
  }
  
  bool parameters::findOne(DatabaseResult& object, std::string& name, std::string package, std::string library) {
    auto valid = [&](DatabaseElement* e) {
      return true;
    };
    return findOneBase(object, name, valid, package, library);
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
    if (!findOneBase(result, name, valid)) {
      exceptions.printError("Could not find declaration of \"" + name + "\"", &identifier->text);
      if (a_verbose) {
	printDatabase();
      }
      return false;
    }
    return true;
  }

  void parameters::findAll(DatabaseResults& objects, std::string& name, std::string package, std::string library) {
    findAllLocal(objects, name, package, library);
    a_global_database.findAll(objects, name, package, library);
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
    debug.functionStart("addObject(name = " + name + ")");
    getActiveClassContainer()->database.add(id, name,  type,
					    arguments, text);
    debug.functionEnd("addObject");
  }
  
  void parameters::addObject(ast::ObjectType id, std::string& name,
			     ast::ObjectValue type,
			     ast::Text* text) {
    debug.functionStart("addObject(name = " + name + ")");
    getActiveClassContainer()->database.add(id, name,  type,
					    text);
    debug.functionEnd("addObject");
  }

    bool parameters::findObject(std::shared_ptr<LocalDatabase>& object, std::string& library, std::string& name, ast::ObjectType type) {
    bool found = false;
    found = a_global_database.findObject(object, library, name, type);
    if (library == "WORK" and !found) {
      for (ClassContainer& i : file_container.content.children) {
	if ((i.name == name) && (type == ast::ObjectType::UNKNOWN || i.type == type)) {
	  object = i.database.local_database;
	  found = true;
	  break;
	}
      }
    }
    return found;
  }
    
  bool parameters::setVisible(std::string& name, std::string package, std::string library) {
    return a_global_database.setVisible(name, package, library);
  }

  void parameters::printDatabase(std::string name) {
    ClassContainer* c = getCurrentClassContainer();
    if (c) {c->database.print(name);}
    a_global_database.print(name);
  }

  void parameters::globalizeClass(ast::Text* text) {
    debug.functionStart("globalizeClass");
    assert(!file_container.content.children.empty());
    ClassContainer& current_top_class = file_container.content.children.back();
    a_global_database.append(current_top_class.database.local_database, file_container.library,
			     current_top_class.name, current_top_class.type, text);
    debug.functionEnd("globalizeClass");
  }

  bool parameters::isLocal(DatabaseResult& object) {
    debug.functionStart("isLocal(object  = " + object.toString() + ")");

    bool local = false;
    if (object.local) {
      local = true;
    } else {
      auto object_hierarchy = object.hierarchy;
      std::list<std::string> current_hierarchy;
      getHierarchy(current_hierarchy);
      const int compare_level = 2;
      if (object_hierarchy->size() >= compare_level && current_hierarchy.size() >= compare_level) {
	auto lst1 = object_hierarchy->begin();
	auto lst2 = current_hierarchy.begin();
	local = true;
	for (int i = 0; i < compare_level; i++) { 
	  std::string s1 = NameConverter::toUpper(*lst1++);
	  std::string s2 = NameConverter::toUpper(*lst2++);
	  debug.debug("s1 = " + s1 + ", s2 = " + s2);
	  if (s1 != s2) {
	    local = false;
	    break;
	  }
	}
      }
    }
    debug.functionEnd("isLocal: " + std::to_string(local));
    return local;
  }

}
