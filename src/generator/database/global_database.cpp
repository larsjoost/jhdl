#include "general.hpp"
#include "global_database.hpp"

namespace generator {
  
  void GlobalDatabase::append(LocalDatabase& d) {
    auto l = map.find(d.getLibrary());
    if (l != map.end()) {
      l->second[d.getPackage()] = d;
    } else {
      std::unordered_map<std::string, LocalDatabase> l;
      l[d.getPackage()] = d;
      map[d.getLibrary()] = l;
    }
  }

  void GlobalDatabase::find(DatabaseResults& results, std::string& name, std::string& package,
                            std::unordered_map<std::string, LocalDatabase>& map) {
    if (package.empty()) {
      for (auto& i : map) {
        i.second.find(results, name);
      }
    } else {
      auto i = map.find(package);
      if (i != map.end()) {
        return i->second.find(results, name);
      }
    }
    
  }

  void GlobalDatabase::find(DatabaseResults& results, std::string& name, std::string package, std::string library) {
    if (library.empty()) {
      for (auto& i : map) {
        find(results, name, package, i.second);
      }
    } else {
      auto i = map.find(library);
      if (i != map.end()) {
        find(results, name, package, i->second);
      }
    }
  }

  bool GlobalDatabase::setVisible(std::string name, std::string package, std::string library) {
    bool found = false;
    DatabaseResults results;
    find(results, name, package, library);
    for (auto& i : results) {
      found = true;
      i.object->visible = true;
    }
    return found;
  }

  bool GlobalDatabase::exists(std::string& name) {
    auto i = map.find(name);
    return (i == map.end()) ? false : true;
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
  

