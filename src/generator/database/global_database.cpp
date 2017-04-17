#include "general.hpp"
#include "global_database.hpp"

namespace generator {
  
  void GlobalDatabase::append(std::string& name, LocalDatabase& d) {
    map[name] = d;
  }

  void GlobalDatabase::find(DatabaseResults& results, std::string& name, std::string package, std::string library) {
    if (library.empty()) {
      for (auto& i : map) {
        i.second.find(results, name, package);
      }
    } else {
      auto i = map.find(library);
      if (i != map.end()) {
        return i->second.find(results, name, package);
      }
    }
  }

  bool GlobalDatabase::setVisible(std::string name, std::string package, std::string library) {
    bool found = false;
    if (!library.empty()) {
      auto i = map.find(name);
      if (i != map.end()) {
        found |= i->second.setVisible(name, package);
      }
    } else {
      for (auto& i : map) {
        found |= i.second.setVisible(name, package);
      }
    }
    return found;
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
  

