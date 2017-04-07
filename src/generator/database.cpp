#include "database.hpp"

namespace generator {
  
  void LocalDatabase::add(std::string& name, std::string& arguments, DatabaseElement& e) {
    map.back().addObject(name, arguments, e);
  }
  
  void LocalDatabase::addObject(std::string& name, std::string& arguments, ast::ObjectType id) {
    DatabaseElement e = {id, false, NULL};
    add(name, arguments, e);
  };

  void LocalDatabase::addObject(std::string& name, ast::ObjectType id) {
    std::string arguments = "";
    addObject(name, arguments, id);
  };

  void NameMap::addObject(std::string& name, std::string& arguments, DatabaseElement& e) {
    auto m = map.find(name);
    if (m != map.end()) {
      m->second.addObject(arguments, e);
    } else {
      ArgumentMap m;
      m.addObject(arguments, e);
      map[name] = m;
    }
  };

  void ArgumentMap::addObject(std::string& arguments, DatabaseElement& e) {
    map[arguments] = e;
  };

  DatabaseElement* LocalDatabase::findObject(std::string& name, std::string& arguments, ast::ObjectType id) {
    return map.back().findObject(name, arguments, id);
  }

  DatabaseElement* LocalDatabase::findObject(std::string& name, int& hierarchyLevel) {
    ast::ObjectType id = (ast::ObjectType)0;
    std::string arguments = "";
    DatabaseElement* e;
    for (auto i : map) {
      e = i.findObject(name, arguments, id, hierarchyLevel);
      if (e) {
        return e;
      }
    }
    return NULL;
  }

  DatabaseElement* LocalDatabase::findObject(std::string& name) {
    std::string arguments = "";
    ast::ObjectType id = (ast::ObjectType)0;
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
      if (m) {
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

  void ArgumentMap::print() {
    for (auto i = map.begin(); i != map.end(); i++) {
      std::cout << "    [ARGUMENTS] = " << i->first << std::endl;
      std::cout << "      visible   = " << (i->second.visible ? "true" : "false") << std::endl;
      std::cout << "      attribute = " << (i->second.attribute ? "true" : "false") << std::endl;
      std::cout << "      type      = " << ast::toString(i->second.id) << std::endl;
    }
  }

  void NameMap::print() {
    std::cout << "[SECTION] = " << section << std::endl;
    for (auto i = map.begin(); i != map.end(); i++) {
      std::cout << "  [NAME] = " << i->first << std::endl;
      i->second.print();
    }
  }

  void LocalDatabase::print() {
    for (auto i = map.begin(); i != map.end(); i++) {
      i->print();
    }
  }

  void LocalDatabase::addAttribute(std::string& name, std::string& arguments,
                                   ast::ObjectType id, ast::Attribute* attribute) {
    DatabaseElement* e = findObject(name, arguments, id);
    if (e) {
      e->attribute = attribute;
    } else {
      std::cerr << "#Error: Could not find object \"" <<  name << "\"" <<
        (arguments.empty() ? "" : " with arguments (" + arguments + ")") << std::endl;
      // print();
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

  std::string LocalDatabase::getParentName(int hierarchy) {
    if (map.size() > hierarchy) {
      auto i = map.end();
      auto p = std::prev(i, hierarchy + 1);
      return p->getSection();
    }
    return "";
  }
  
  void GlobalDatabase::add(std::string& name, LocalDatabase& d) {
    map[name] = d;
  }
  
  DatabaseElement* GlobalDatabase::findObject(std::string& name, std::string& arguments,
                                              ast::ObjectType id, std::string& location) {
    if (location.empty()) {
      for (auto i = map.begin(); i != map.end(); i++) {
        DatabaseElement* e = i->second.findObject(name, arguments, id);
        if (e) {
          location = i->first;
          return e;
        }
      }
    } else {
      auto i = map.find(location);
      if (i != map.end()) {
        return i->second.findObject(name, arguments, id);
      }
    }
    return NULL;
  }

  bool GlobalDatabase::setAllVisible(std::string& name) {
    auto i = map.find(name);
    if (i != map.end()) {
      i->second.setAllVisible();
      return true;
    }
    return false;
  }
  
  bool GlobalDatabase::setVisible(std::string& name, std::string& subname) {
    auto i = map.find(name);
    if (i != map.end()) {
      i->second.setVisible(subname);
      return true;
    }
    return false;
  }

  bool GlobalDatabase::exists(std::string& name) {
    auto i = map.find(name);
    return (i == map.end()) ? false : true;
  }

  void GlobalDatabase::print(std::string name) {
    if (name.empty()) {
      for (auto i = map.begin(); i != map.end(); i++) {
        std::cout << "[GLOBAL] = " << i->first << std::endl;
        i->second.print();
      }
    } else {
      auto i = map.find(name);
      if (i != map.end()) {
        std::cout << "[GLOBAL] = " << i->first << std::endl;
        i->second.print();
      } else {
        std::cerr << "Did not find " + name + " in global database" << std::endl;
      }
             
    }
  }
  
}
  

