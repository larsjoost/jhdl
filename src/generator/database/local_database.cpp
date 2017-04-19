#include "database.hpp"

namespace generator {
  
  void LocalDatabase::add(std::string& name, DatabaseElement& e) {
    map.back().add(name, e);
  }
  
  void LocalDatabase::find(DatabaseResults& results, std::string& name, std::string package) {
    if (package.empty() || package == this->package) {
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
  }

  bool LocalDatabase::setVisible(std::string name, std::string package) {
    bool found = false;
    if (package.empty() || package == this->package) {
      for (auto& i : map) {
        found |= i.setVisible(name);
      }
    }
    return found;
  }

  void LocalDatabase::print() {
    for (auto& i : map) {
      i.print();
    }
  }

  void LocalDatabase::add(ast::ObjectType id, std::string& name, ast::ObjectValueContainer type, ast::ObjectArguments arguments) {
    DatabaseElement e = {id, library, package, name, arguments, type, false, NULL, NULL, NULL};
    add(name, e);
  }

  void LocalDatabase::addAttribute(std::string& name, ast::ObjectArguments& arguments, ast::Attribute* attribute) {
    DatabaseResults results;
    find(results, name);
    if (!results.empty()) {
      for (auto& i : results) {
        if (i.object->arguments.equals(arguments)) {
          i.object->attribute = attribute;
        }
      }
    } else {
      exceptions.printError("Could not find object \"" +  name + "\"");
    }
  };

  void LocalDatabase::addFunction(std::string& name, ast::ObjectArguments& arguments,
                                  ast::ObjectValueContainer returnType,
                                  ast::FunctionDeclaration* function) {
    DatabaseElement e = {ast::FUNCTION, library, package, name, arguments, returnType, false, NULL, function, NULL};
    add(name, e);
  };

  void LocalDatabase::addProcedure(std::string& name, ast::ObjectArguments& arguments,
                                   ast::ProcedureDeclaration* procedure) {
    ast::ObjectValueContainer c = ast::ObjectValueContainer(ast::NONE);
    DatabaseElement e = {ast::PROCEDURE, library, package, name, arguments, c, false, NULL, NULL, procedure};
    add(name, e);
  };

  void LocalDatabase::descendHierarchy(std::string& name) {
    NameMap m(name);
    map.push_front(m);
  }
  
  void LocalDatabase::ascendHierarchy() {
    map.pop_front();
  }

  /*
  std::string LocalDatabase::getParentName(int hierarchy) {
    if (map.size() > hierarchy) {
      auto i = map.end();
      auto p = std::prev(i, hierarchy + 1);
      return p->getSection();
    }
    return "";
  }
  */
  
}
  

