#include <cassert>

#include "general.hpp"
#include "global_database.hpp"

namespace generator {
  
  void GlobalDatabase::append(LocalDatabase& d) {
    auto l = map.find(d.getLibrary());
    if (l != map.end()) {
      if (l->second.find(d.getName()) == l->second.end()) {
        l->second[d.getName()] = d;
      } else {
        exceptions.printError(ast::toString(d.getType()) + " " + d.getLibrary() + "." + d.getName() + " is already defined");
      }
    } else {
      std::unordered_map<std::string, LocalDatabase> l;
      l[d.getName()] = d;
      map[d.getLibrary()] = l;
    }
  }

  bool GlobalDatabase::find(DatabaseResults& results, std::string& name, std::string package, std::string library) {
    auto func = [&](LocalDatabase& l) {
      l.find(results, name, false);
    };
    return traverse(package, library, func);
  }

  bool GlobalDatabase::setVisible(std::string name, std::string package, std::string library) {
    auto func = [&](LocalDatabase& l) {
      l.setVisible(name);
    };
    return traverse(package, library, func);
  }

  bool GlobalDatabase::exists(std::string& library, std::string& package) {
    auto i = map.find(library);
    if (i != map.end()) {
      auto j = i->second.find(package);
      return j == i->second.end() ? false : true;
    }
    return false;
  }

  void GlobalDatabase::print(std::unordered_map<std::string, LocalDatabase>& m) {
    for (auto& i : m) {
      std::cout << "[GLOBAL PACKAGE] = " << i.first << std::endl;
      i.second.print();
    }
  }
  
  void GlobalDatabase::print(std::string name) {
    if (name.empty()) {
      for (auto& i : map) {
        std::cout << "[GLOBAL LIBRARY] = " << i.first << std::endl;
        print(i.second);
      }
    } else {
      auto i = map.find(name);
      if (i != map.end()) {
        std::cout << "[GLOBAL] = " << i->first << std::endl;
        print(i->second);
      } else {
        std::cerr << "Did not find " + name + " in global database" << std::endl;
      }
             
    }
  }
  
}
  

