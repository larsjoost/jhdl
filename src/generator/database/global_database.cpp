#include <cassert>

#include "general.hpp"
#include "global_database.hpp"

namespace generator {
  
  void GlobalDatabase::append(LocalDatabase& d) {
    auto l = a_map.find(d.getLibrary());
    if (l != a_map.end()) {
      if (l->second.find(d.getName()) == l->second.end()) {
        l->second[d.getName()] = d;
      } else {
        exceptions.printError(ast::toString(d.getType()) + " " + d.getLibrary() + "." + d.getName() + " is already defined");
      }
    } else {
      std::unordered_map<std::string, LocalDatabase> l;
      l[d.getName()] = d;
      a_map[d.getLibrary()] = l;
    }
  }

  LocalDatabase* GlobalDatabase::find(std::string& library, const std::string& name) {
    auto l = a_map.find(library);
    if (l != a_map.end()) {
      auto x = l->second.find(name);
      if (x != l->second.end()) {
        return &x->second;
      }
    }
    return NULL;
  }

  bool GlobalDatabase::find(DatabaseResults& results, const std::string& name, std::string package, std::string library) {
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
    return (find(library, package) ? true : false);
  }

  void GlobalDatabase::print(std::unordered_map<std::string, LocalDatabase>& m) {
    for (auto& i : m) {
      std::cout << "[GLOBAL] name = " << i.first << std::endl;
      i.second.print();
    }
  }
  
  void GlobalDatabase::print(std::string name) {
    if (name.empty()) {
      for (auto& i : a_map) {
        std::cout << "[GLOBAL] library = " << i.first << std::endl;
        print(i.second);
      }
    } else {
      auto i = a_map.find(name);
      if (i != a_map.end()) {
        std::cout << "[GLOBAL] library = " << i->first << std::endl;
        print(i->second);
      } else {
        std::cerr << "Did not find " + name + " in global database" << std::endl;
      }
             
    }
  }
  
}
  

