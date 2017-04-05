#include "database.hpp"

namespace generator {
  
  void LocalDatabase::add(std::string& name, std::string& arguments, DatabaseElement& e) {
    map.back().addObject(name, arguments, e);
  }
  
  void LocalDatabase::addObject(std::string& name, std::string& arguments, ast::ObjectType id) {
    DatabaseElement e = {id, false, NULL};
    add(name, arguments, e);
  };

  void NameMap::addObject(std::string& name, std::string& arguments, DatabaseElement& e) {
    ArgumentMap m;
    m.addObject(arguments, e);
    map[name] = m;
  };

  void ArgumentMap::addObject(std::string& arguments, DatabaseElement& e) {
    map[arguments] = e;
  };

  DatabaseElement* LocalDatabase::findObject(std::string& name, std::string& arguments, ast::ObjectType id) {
    return map.back().findObject(name, arguments, id);
  }

  DatabaseElement* LocalDatabase::findObject(std::string& name, int& hierarchyLevel) {
    ast::ObjectType id;
    return map.findObject(name, arguments, id, hierarchyLevel);
  }

  DatabaseElement* LocalDatabase::findObject(std::string& name) {
    std::string arguments = "";
    ast::ObjectType id = 0;
    return findObject(name, arguments, id);
  }

  DatabaseElement* NameMap::findObject(std::string& name, std::string& arguments, ast::ObjectType id) {
    int h;
    return findObject(name, arguments, id, h);
  }

  DatabaseElement* NameMap::findObject(std::string& name, std::string& arguments, ast::ObjectType id, int& hierarchyLevel) {
    hierarchyLevel = 0;
    for (auto i = map.begin(); i != map.end(); i++) {
      auto m = i->second.findObject(arguments, id);
      if (m != NULL) {
        return m;
      }
      hierarchyLevel++;
    }
    return NULL;
  }

  DatabaseElement* ArgumentMap::findObject(std::string& arguments, ast::ObjectType id) {
    auto m = map.find(arguments);
    if (m != map.end()) {
      return &m->second;
    }
    return NULL;
  }

  void LocalDatabase::setVisible(std::string& name) {
    for (auto i = map.begin(); i != map.end(); i++) {
      i->setVisible(name);
    }
  }
  
  void NameMap::setVisible(std::string& name) {
    auto a = map.find(name);
    a->second.setAllVisible();
  }

  void LocalDatabase::setAllVisible() {
    for (auto i = map.begin(); i != map.end(); i++) {
      i->setAllVisible();
    }
  }

  void NameMap::setAllVisible() {
    for (auto i = map.begin(); i != map.end(); i++) {
      i->second.setAllVisible();
    }
  }
  
  void ArgumentMap::setAllVisible() {
    for (auto i = map.begin(); i != map.end(); i++) {
      i->second.visible = true;
    }
  }

  void LocalDatabase::addAttribute(std::string& name, std::string& arguments,
                                   ast::ObjectType id, ast::Attribute* attribute) {
    DatabaseElement* e = findObject(name, arguments, id);
    if (e) {
      e->attribute = attribute;
    } else {
      std::cerr << "#Error: Could not find object \"" <<  name << "\"" <<
        (arguments.empty() ? "" : " with arguments " + arguments) << std::endl;
    }
  };

  void LocalDatabase::addFunction(std::string& name, std::string& arguments,
                                  ast::FunctionDeclaration* function) {
    DatabaseElement e = {ast::FUNCTION, false, NULL, function, NULL};
    add(name, arguments, e);
  };

  void LocalDatabase::addProcedure(std::string& name, std::string& arguments,
                                   ast::ProcedureDeclaration* procedure) {
    DatabaseElement e = {ast::PROCEDURE, false, NULL, NULL, procedure};
    add(name, arguments, e);
  };

  void LocalDatabase::descendHierarchy(std::string& name) {
    NameMap m(name);
    map.push_back(m);
  }
  
  void LocalDatabase::ascendHierarchy() {
    map.pop_back();
  }

  std::string LocalDatabase::getParentName() {
    if (map.size() > 1) {
      return map.end()[-2].name;
    }
    return ""
  }
  
  void GlobalDatabase::add(std::string& name, LocalDatabase& d) {
    map[name] = d;
  }
  
  DatabaseElement* GlobalDatabase::findObject(std::string& name, std::string& arguments, ast::ObjectType id, std::string& location) {
    for (auto i = map.begin(); i != map.end(); i++) {
      DatabaseElement* e = i->second.findObject(name, arguments, id);
      if (e) {
        location = i->first;
        return e;
      }
    }
    return NULL;
  }

  
}
  

