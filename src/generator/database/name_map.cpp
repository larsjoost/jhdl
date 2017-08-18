#include "name_map.hpp"

namespace generator {

  void NameMap::add(std::string& name, DatabaseElement& e) {
    auto m = a_map.find(name);
    if (m != a_map.end()) {
      m->second.push_back(e);
    } else {
      std::list<DatabaseElement> m;
      m.push_back(e);
      a_map[name] = m;
    }
  };

  void NameMap::add(NameMap& other) {
    a_map.insert(other.a_map.begin(), other.a_map.end());
  }

  std::list<DatabaseElement>* NameMap::find(const std::string& name) {
    auto m = a_map.find(name);
    if (m != a_map.end()) {
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
      for (auto i = a_map.begin(); i != a_map.end(); i++) {
        found = true;
        setVisible(i->second);
      }
    } else {
      auto a = a_map.find(name);
      if (a != a_map.end()) {
        found = true;
        setVisible(a->second);
      }
    }
    return found;
  }

  void NameMap::print() {
    for (auto i = a_map.begin(); i != a_map.end(); i++) {
      for (auto j = i->second.begin(); j != i->second.end(); j++) {
        std::cout << "  [NAME] = " << i->first << std::endl;
        j->print();
      }
    }
  }

}
  

