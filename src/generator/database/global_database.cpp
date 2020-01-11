#include <cassert>

#include "general.hpp"
#include "global_database.hpp"

namespace generator {
  
  void GlobalDatabase::append(std::shared_ptr<LocalDatabase>& d, std::string& library, std::string& object_name) {
    debug.functionStart("append(library = " + library + ", object_name = " + object_name + ")");
    assert(library.size() > 0);
    assert(object_name.size() > 0);
    auto l = a_map.find(library);
    if (l != a_map.end()) {
      if (l->second.find(object_name) == l->second.end()) {
        l->second[object_name] = d;
      } else {
        exceptions.printError(library + "." + object_name + " is already defined");
      }
    } else {
      std::unordered_map<std::string, std::shared_ptr<LocalDatabase>> x;
      x[object_name] = d;
      a_map[library] = x;
    }
    if (debug.isVerbose()) {
      print();
    }
    debug.functionEnd("append");
  }

  std::shared_ptr<LocalDatabase>* GlobalDatabase::find(std::string& library, const std::string& name) {
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
    auto func = [&](std::shared_ptr<LocalDatabase>& l) {
      l->find(results, name);
    };
    return traverse(package, library, func);
  }

  bool GlobalDatabase::setVisible(std::string name, std::string package, std::string library) {
    std::string n = (name == "ALL") ? "" : name;
    auto func = [&](std::shared_ptr<LocalDatabase>& l) {
      l->setVisible(n);
    };
    return traverse(package, library, func);
  }

  bool GlobalDatabase::exists(std::string& library, std::string& package, std::string object_name) {
    DatabaseResults d;
    return (find(d, object_name, library, package) ? true : false);
  }

  void GlobalDatabase::print(std::unordered_map<std::string, std::shared_ptr<LocalDatabase>>& m) {
    for (auto& i : m) {
      std::cout << "[GLOBAL] name = " << i.first << std::endl;
      i.second->print();
    }
  }
  
  void GlobalDatabase::print(std::string library) {
    if (library.empty()) {
      for (auto& i : a_map) {
        std::cout << "[GLOBAL] library = " << i.first << std::endl;
        print(i.second);
      }
    } else {
      auto i = a_map.find(library);
      if (i != a_map.end()) {
        std::cout << "[GLOBAL] library = " << i->first << std::endl;
        print(i->second);
      } else {
        std::cerr << "Did not find " + library + " in global database" << std::endl;
      }
             
    }
  }
  
}
  

