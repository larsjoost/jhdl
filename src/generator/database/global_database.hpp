#ifndef _GLOBAL_DATABASE_HPP_
#define _GLOBAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"
#include "local_database.hpp"

namespace generator {

  class GlobalDatabase {
    
    std::unordered_map<std::string, std::unordered_map<std::string, LocalDatabase>> map;

    void print(std::unordered_map<std::string, LocalDatabase>& m);

    template<typename Func>
    bool traverse(std::string& package, std::string& library, Func func);
    template<typename Func>
    bool traverse(std::unordered_map<std::string, LocalDatabase>& m, std::string& package, Func func);
    
  public:
    void append(LocalDatabase& d);
    bool find(DatabaseResults& results, std::string& name, std::string package = "", std::string library = "");
    bool setVisible(std::string name = "", std::string package = "", std::string library = "");
    bool exists(std::string& library, std::string& package);
    void print(std::string name = "");
  };

  template<typename Func>
  bool GlobalDatabase::traverse(std::string& package, std::string& library, Func func) {
    bool found = false;
    if (library.empty()) {
      for (auto& i : map) {
        found |= traverse(i.second, package, func);
      }
    } else {
      auto i = map.find(library);
      if (i != map.end()) {
        found |= traverse(i->second, package, func);
      }
    }
    return found;
  }
  
  template<typename Func>
  bool GlobalDatabase::traverse(std::unordered_map<std::string, LocalDatabase>& m, std::string& package, Func func) {
    bool found = false;
    if (package.empty()) {
      for (auto& i : m) {
        func(i.second);
        found = true;
      }
    } else {
      auto i = m.find(package);
      if (i != m.end()) {
        func(i->second);
        found = true;
      }
    }
    return found;
  }
  
}

#endif
