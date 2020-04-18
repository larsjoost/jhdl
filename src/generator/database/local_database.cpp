#include <cassert>

#include "database.hpp"

namespace generator {

  void LocalDatabase::findAll(DatabaseResults& results, const std::string& name) {
    auto action_callback =
      [&](DatabaseResult& r) {};
    findAll(results, name, action_callback);
  }

  void LocalDatabase::add(std::string& name, DatabaseElement& e) {
    m_debug.functionStart("add(name = " + name + ")", false, __FILE__, __LINE__);
    m_debug.debug("e = " + e.toString());
    a_content.add(name, e);
    m_debug.functionEnd("add");
  }
  
  bool LocalDatabase::setVisible(std::string name) {
    return a_content.setVisible(name);
  }

  void LocalDatabase::print() {
    a_content.print();
  }

  void LocalDatabase::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                          ast::Text* text) {
    m_debug.functionStart("add(name = " + name + ")", false, __FILE__, __LINE__);
    ast::ObjectInterface a;
    add(id, name, type, a, text);
    m_debug.functionEnd("add");
  }

  void LocalDatabase::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                          ast::ObjectInterface interface, ast::Text* text) {
    m_debug.functionStart("add(name = " + name + ")", false, __FILE__, __LINE__);
    DatabaseElement e;
    e.id = id;
    e.name = name;
    e.interface = interface;
    e.type = type;
    e.text = text;
    add(name, e);
    m_debug.functionEnd("add");
  }

  void LocalDatabase::addAttribute(std::string& name, ast::ObjectInterface& interface, ast::ObjectType id,
                                   ast::Attribute* attribute, ast::Text* text) {
    m_debug.functionStart("addAttribute(name = " + name + ")", false, __FILE__, __LINE__);
    if (m_debug.isVerbose()) {
      print();
    }
    bool found = false;
    DatabaseResults results;
    findAll(results, name);
    for (auto& i : results) {
      m_debug.debug("i = " + i.toString());
      if (id == i.object->id && (interface.empty() || i.object->interface.exactMatch(interface))) {
	assert(!i.object->attribute);
	i.object->attribute = attribute;
	if (found) {
	  a_exceptions.printError("Attribute " + toString(id) + ": " +  name + "(" + interface.toString() + ")" + " already exists", text);
	}
	found = true;
      }
    }
    if (!found) {
      a_exceptions.printError("Could not find " + toString(id) + ": " +  name + "(" + interface.toString() + ")", text);
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
  
  void LocalDatabase::addFunction(ast::ObjectType type, std::string& name, ast::ObjectInterface& interface,
                                  ast::ObjectValueContainer returnType,
                                  ast::FunctionDeclaration* function,
                                  ast::Text* text) {
    DatabaseElement e;
    e.id = type;
    e.name = name;
    e.interface = interface;
    e.type = returnType;
    e.function = function;
    e.text = text;
    add(name, e);
  };

}
  

