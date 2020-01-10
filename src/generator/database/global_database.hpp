#ifndef _GLOBAL_DATABASE_HPP_
#define _GLOBAL_DATABASE_HPP_

#include <unordered_map>
#include <memory>
#include <list>

#include "../../exceptions/exceptions.hpp"

#include "general.hpp"
#include "local_database.hpp"

namespace generator {

  class GlobalDatabase {
    
    Exceptions exceptions;

    std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<LocalDatabase>>> a_map;

    void print(std::unordered_map<std::string, std::shared_ptr<LocalDatabase>>& m);

    template<typename Func>
    bool traverse(std::string& package, std::string& library, Func func);
    template<typename Func>
    bool traverse(std::unordered_map<std::string, std::shared_ptr<LocalDatabase>>& m, std::string& package, Func func);
    
  public:
    void append(std::shared_ptr<LocalDatabase>& d, std::string& library, std::string& object_name);
    bool find(DatabaseResults& results, const std::string& name, std::string package = "", std::string library = "");
    std::shared_ptr<LocalDatabase>* find(std::string& library, const std::string& name);
    bool setVisible(std::string name = "", std::string package = "", std::string library = "");
    bool exists(std::string& library, std::string& package, std::string name = "");
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
  bool GlobalDatabase::traverse(std::unordered_map<std::string, std::shared_ptr<LocalDatabase>>& m, std::string& package, Func func) {
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
