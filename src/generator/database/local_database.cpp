#include <cassert>

#include "database.hpp"

namespace generator {
  
  void LocalDatabase::initialize(std::string& library, std::string& name, ast::ObjectType type) {
    assert(a_map.empty());
    a_library = library;
    a_name = name;
    a_type = type;
  }

  std::string LocalDatabase::getLibrary() {
    return a_library;
  }

  std::string LocalDatabase::getName() {
    return a_name;
  }

  ast::ObjectType LocalDatabase::getType() {
    return a_type;
  }

  void LocalDatabase::add(std::string& name, DatabaseElement& e) {
    assert(!a_map.empty());
    e.hierarchyLevel = getHierarchyLevel();
    a_map.front().add(name, e);
  }
  
  void LocalDatabase::find(DatabaseResults& results, std::string& name, bool local) {
    for (auto i = a_map.begin(); i != a_map.end(); i++) {
      std::list<DatabaseElement>* e = i->find(name);
      if (e) {
        for (auto& j : *e) {
          DatabaseResult r;
	  r.local = local;
          r.object = &j;
          results.push_back(r);
        }
      }
    }
  }

  bool LocalDatabase::setVisible(std::string name) {
    bool found = false;
    for (auto& i : a_map) {
      found |= i.setVisible(name);
    }
    return found;
  }

  void LocalDatabase::print() {
    int hierarchy_level = 0;
    std::cout << "  [LOCAL] library = " << a_library <<
      ", name = " << a_name <<
      ", type = " << ast::toString(a_type) << std::endl;
    for (auto i = a_map.begin(); i != a_map.end(); i++) {
      std::cout << "  # Hierarchy level = " << hierarchy_level << std::endl;
      i->print();
      hierarchy_level++;
    }
  }

  void LocalDatabase::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                          ast::ObjectArguments arguments, ast::Text* text) {
    DatabaseElement e = {id, a_library, a_type, a_name, name, arguments, type, false, NULL, NULL, NULL, text};
    add(name, e);
  }

  void LocalDatabase::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id,
                                   ast::Attribute* attribute, ast::Text* text) {
    DisableDebug debug = DisableDebug("LocalDatabase");
    debug.functionStart("addAttribute");
    if (id == a_type) {
      if (name == a_name) {
        a_package_attributes.push_back(attribute);
      } else {
        a_exceptions.printError("Could not find " + ast::toString(id) + "\"" + name + "\"", text);
      }
    } else {
      bool found = false;
      DatabaseResults results;
      find(results, name);
      for (auto& i : results) {
        debug.debug("i = " + i.toString());
	if (id == i.object->id && (arguments.empty() || i.object->arguments.equals(arguments))) {
	  i.object->attribute = attribute;
	  found = true;
	}
      }
      if (!found) {
        a_exceptions.printError("Could not find " + toString(id) + ": " +  name + "(" + arguments.toString() + ")", text);
	printAllObjects(name);
        if (a_verbose) {
          print();
        }
      }
    }
    debug.functionEnd("addAttribute");
  };

  void LocalDatabase::printAllObjects(std::string& name) {
      DatabaseResults results;
      find(results, name);
      if (!results.empty()) { 
	std::cerr << "Found the following objects with name \"" + name + "\":" << std::endl;
      	for (auto& i : results) {
	  std::cerr << i.object->toString() << std::endl;
	}
      } else {
	std::cerr << "Found no objects with name " + name << std::endl;
      }
  }
  
  void LocalDatabase::addFunction(std::string& name, ast::ObjectArguments& arguments,
                                  ast::ObjectValueContainer returnType,
                                  ast::FunctionDeclaration* function,
                                  ast::Text* text) {
    DatabaseElement e = {ast::ObjectType::FUNCTION, a_library, a_type, a_name, name, arguments,
                         returnType, false, NULL, function, NULL, text};
    add(name, e);
  };

  void LocalDatabase::addProcedure(std::string& name, ast::ObjectArguments& arguments,
                                   ast::ProcedureDeclaration* procedure,
                                   ast::Text* text) {
    ast::ObjectValueContainer c = ast::ObjectValueContainer(ast::ObjectValue::NONE);
    DatabaseElement e = {ast::ObjectType::PROCEDURE, a_library, a_type, a_name, name, arguments, c, false, NULL, NULL, procedure, text};
    add(name, e);
  };

  void LocalDatabase::add(LocalDatabase* other) {
    NameMap& m = a_map.front();
    assert(other->getHierarchyLevel() == 0);
    m.add(other->a_map.front());
  }
  
  void LocalDatabase::descendHierarchy(std::string& name) {
    NameMap m(name);
    a_map.push_front(m);
  }

  void LocalDatabase::ascendHierarchy() {
    a_map.pop_front();
  }

  int LocalDatabase::getHierarchyLevel() {
    return a_map.size() - 1; 
  }
  
  std::string LocalDatabase::getParentName() {
    std::string name = a_name;
    if (a_map.size() > 1) {
      auto p = std::next(a_map.begin());
      name = p->getSection();
    }
    return name;
  }
  
}
  

