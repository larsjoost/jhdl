#include "name_map.hpp"

namespace generator {

  std::string NameMap::getSection() {
    return section;
  }
  
  void NameMap::add(std::string& name, DatabaseElement& e) {
    auto m = map.find(name);
    if (m != map.end()) {
      m->second.push_back(e);
    } else {
      std::list<DatabaseElement> m;
      m.push_back(e);
      map[name] = m;
    }
  };

  std::list<DatabaseElement>* NameMap::find(std::string& name) {
    auto m = map.find(name);
    if (m != map.end()) {
      return &m->second;
    }
    return NULL;
  }

  void NameMap::setVisible(std::list<DatabaseElement>& l) {
    for (auto i = l.begin(); i != l.end(); i++) {
      i->visible = true;
    }
  }
  
  bool NameMap::setVisible(std::string name) {
    bool found = false;
    if (name.empty()) {
      for (auto i = map.begin(); i != map.end(); i++) {
        found = true;
        setVisible(i->second);
      }
    } else {
      auto a = map.find(name);
      if (a != map.end()) {
        found = true;
        setVisible(a->second);
      }
    }
    return found;
  }

  void NameMap::print() {
    for (auto i = map.begin(); i != map.end(); i++) {
      for (auto j = i->second.begin(); j != i->second.end(); j++) {
        std::cout << "  [NAME] = " << i->first << std::endl;
        j->print();
      }
    }
  }

  
}
  

