#include <cassert>

#include "database.hpp"

namespace generator {

  void LocalDatabase::findAll(DatabaseResults& results, const std::string& name) {
    auto action_callback =
      [&](DatabaseResult& r) {};
    findAll(results, name, action_callback);
  }

  void LocalDatabase::add(std::string& name, DatabaseElement& e) {
    a_content.add(name, e);
  }
  
  bool LocalDatabase::setVisible(std::string name) {
    return a_content.setVisible(name);
  }

  void LocalDatabase::print() {
    a_content.print();
  }

  void LocalDatabase::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                          ast::ObjectArguments arguments, ast::Text* text) {
    DatabaseElement e = {id, name, arguments, type, false, NULL, NULL, text};
    add(name, e);
  }

  void LocalDatabase::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id,
                                   ast::Attribute* attribute, ast::Text* text) {
    Debug<false> debug(this);
    m_debug.functionStart("addAttribute(name = " + name + ")");
    if (m_debug.isVerbose()) {
      print();
    }
    bool found = false;
    DatabaseResults results;
    findAll(results, name);
    for (auto& i : results) {
      m_debug.debug("i = " + i.toString());
      if (id == i.object->id && (arguments.empty() || i.object->arguments.ExactMatch(arguments))) {
	assert(!i.object->attribute);
	i.object->attribute = attribute;
	if (found) {
	  a_exceptions.printError("Attribute " + toString(id) + ": " +  name + "(" + arguments.toString() + ")" + " already exists", text);
	}
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
    if (m_debug.isVerbose()) {
      print();
    }
    m_debug.functionEnd("addAttribute");
  };

  void LocalDatabase::printAllObjects(std::string& name) {
      DatabaseResults results;
      findAll(results, name);
      if (!results.empty()) { 
	std::cerr << "Found the following objects with name \"" + name + "\":" << std::endl;
      	for (auto& i : results) {
	  std::cerr << i.object->toString() << std::endl;
	}
      } else {
	std::cerr << "Found no objects with name " + name << std::endl;
      }
  }
  
  void LocalDatabase::addFunction(ast::ObjectType type, std::string& name, ast::ObjectArguments& arguments,
                                  ast::ObjectValueContainer returnType,
                                  ast::FunctionDeclaration* function,
                                  ast::Text* text) {
    DatabaseElement e = {type, name, arguments,
                         returnType, false, NULL, function, text};
    add(name, e);
  };

}
  

