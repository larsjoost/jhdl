#ifndef _GLOBAL_DATABASE_HPP_
#define _GLOBAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "../../exceptions/exceptions.hpp"

#include "general.hpp"
#include "local_database.hpp"

namespace generator {

  class GlobalDatabase {
    
    Exceptions exceptions;

    std::unordered_map<std::string, std::unordered_map<std::string, LocalDatabase>> a_map;

    void print(std::unordered_map<std::string, LocalDatabase>& m);

    template<typename Func>
    bool traverse(std::string& package, std::string& library, Func func);
    template<typename Func>
    bool traverse(std::unordered_map<std::string, LocalDatabase>& m, std::string& package, Func func);
    
  public:
    void append(LocalDatabase& d);
    bool find(DatabaseResults& results, std::string& name, std::string package = "", std::string library = "");
    LocalDatabase* find(std::string& name, std::string& library);
    bool setVisible(std::string name = "", std::string package = "", std::string library = "");
    bool exists(std::string& library, std::string& package);
    void print(std::string name = "");
  };

  template<typename Func>
  bool GlobalDatabase::traverse(std::string& package, std::string& library, Func func) {
    bool found = false;
    if (library.empty()) {
      for (auto& i : a_map) {
        found |= traverse(i.second, package, func);
      }
    } else {
      auto i = a_map.find(library);
      if (i != a_map.end()) {
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
