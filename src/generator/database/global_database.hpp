#ifndef _GLOBAL_DATABASE_HPP_
#define _GLOBAL_DATABASE_HPP_

#include <unordered_map>
#include <memory>
#include <list>

#include "../../exceptions/exceptions.hpp"

#include "general.hpp"
#include "local_database.hpp"
#include "../../debug/debug.hpp"

namespace generator {

  class GlobalDatabase {
    
    Debug<true> debug;

    Exceptions exceptions;

    struct Object {
      ast::ObjectType type;
      std::shared_ptr<LocalDatabase> database;
    };
    
    std::unordered_map<std::string, std::unordered_map<std::string, Object>> a_map;

    void print(std::unordered_map<std::string, Object>& m);

    template<typename Func>
    bool traverse(std::string& package, std::string& library, Func func);
    template<typename Func>
    bool traverse(std::unordered_map<std::string, Object>& m, std::string& package, Func func);
    
  public:
    GlobalDatabase() : debug("GlobalDatabase") {};
    void append(std::shared_ptr<LocalDatabase>& d, std::string& library, std::string& object_name, ast::ObjectType type);
    bool findAll(DatabaseResults& results, const std::string& name, std::string& package, std::string& library);
    bool findObject(std::shared_ptr<LocalDatabase>& object, std::string& library, const std::string& name, ast::ObjectType type = ast::ObjectType::UNKNOWN);
    bool setVisible(std::string name = "", std::string package = "", std::string library = "");
    void print(std::string library = "");
  };

  template<typename Func>
  bool GlobalDatabase::traverse(std::string& package, std::string& library, Func func) {
    debug.functionStart("traverse(package = " + package + ", library = " + library + ")");
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
    debug.functionEnd("traverse: " + std::to_string(found));
    return found;
  }
  
  template<typename Func>
  bool GlobalDatabase::traverse(std::unordered_map<std::string, Object>& m, std::string& package, Func func) {
    debug.functionStart("traverse(package = " + package + ")");
    bool found = false;
    if (package.empty()) {
      for (auto& i : m) {
        func(i.second.database);
        found = true;
      }
    } else {
      auto i = m.find(package);
      if (i != m.end()) {
        func(i->second.database);
        found = true;
      }
    }
    debug.functionEnd("traverse: " + std::to_string(found));
    return found;
  }

}

#endif
