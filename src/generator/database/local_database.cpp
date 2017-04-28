#include <cassert>

#include "database.hpp"

namespace generator {
  
  void LocalDatabase::setLibrary(std::string& name) {
    library = name;
  }

  std::string LocalDatabase::getLibrary() {
    assert(library.size() > 0);
    return library;
  }

  void LocalDatabase::setPackage(std::string& name) {
    package = name;
  }

  std::string LocalDatabase::getPackage() {
    assert(package.size() > 0);
    return package;
  }

  void LocalDatabase::add(std::string& name, DatabaseElement& e) {
    map.back().add(name, e);
  }
  
  void LocalDatabase::find(DatabaseResults& results, std::string& name) {
    int hierarchyLevel = 0;
    for (auto i = map.begin(); i != map.end(); i++) {
      std::list<DatabaseElement>* e = i->find(name);
      if (e) {
        for (auto& j : *e) {
          DatabaseResult r;
          r.hierarchyLevel = hierarchyLevel;
          r.object = &j;
          results.push_back(r);
        }
      }
      hierarchyLevel++;
    }
  }

  bool LocalDatabase::setVisible(std::string name) {
    bool found = false;
    for (auto& i : map) {
      found |= i.setVisible(name);
    }
    return found;
  }

  void LocalDatabase::print() {
    for (auto& i : map) {
      i.print();
    }
  }

  void LocalDatabase::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type,
                          ast::ObjectArguments arguments, ast::Text* text) {
    DatabaseElement e = {id, library, package, name, arguments, type, false, NULL, NULL, NULL, text};
    add(name, e);
  }

  void LocalDatabase::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::ObjectType id,
                                   ast::Attribute* attribute, ast::Text* text) {
    if (id == ast::PACKAGE) {
      if (name == package) {
        packageAttributes.push_back(attribute);
      } else {
        exceptions.printError("Could not find package \"" + name + "\"", text);
      }
    } else {
      bool found = false;
      DatabaseResults results;
      find(results, name);
      if (!results.empty()) {
        for (auto& i : results) {
          if (id == i.object->id && i.object->arguments.equals(arguments)) {
            i.object->attribute = attribute;
            found = true;
          }
        }
      }
      if (!found) {
        exceptions.printError("Could not find " + toString(id) + ": " +  name + "(" + arguments.toString() + ")", text);
      }
    }
  };

  void LocalDatabase::addFunction(std::string& name, ast::ObjectArguments& arguments,
                                  ast::ObjectValueContainer returnType,
                                  ast::FunctionDeclaration* function,
                                  ast::Text* text) {
    DatabaseElement e = {ast::FUNCTION, library, package, name, arguments, returnType, false, NULL, function, NULL, text};
    add(name, e);
  };

  void LocalDatabase::addProcedure(std::string& name, ast::ObjectArguments& arguments,
                                   ast::ProcedureDeclaration* procedure,
                                   ast::Text* text) {
    ast::ObjectValueContainer c = ast::ObjectValueContainer(ast::NONE);
    DatabaseElement e = {ast::PROCEDURE, library, package, name, arguments, c, false, NULL, NULL, procedure, text};
    add(name, e);
  };

  void LocalDatabase::descendHierarchy(std::string& name) {
    NameMap m(name);
    map.push_front(m);
  }
  
  void LocalDatabase::ascendHierarchy() {
    map.pop_front();
  }

  std::string LocalDatabase::getParentName(int hierarchy) {
    if (map.size() > hierarchy) {
      auto i = map.end();
      auto p = std::prev(i, hierarchy + 1);
      return p->getSection();
    }
    return "";
  }
  
}
  

