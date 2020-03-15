#ifndef _GLOBAL_DATABASE_HPP_
#define _GLOBAL_DATABASE_HPP_

#include <unordered_map>
#include <memory>
#include <list>


#include "general.hpp"
#include "local_database.hpp"

#include "../../exceptions/exceptions.hpp"
#include "../../debug/debug.hpp"
#include "../../ast/text.hpp"

namespace generator {

  class GlobalDatabase {
    
    Debug<false> m_debug;

    Exceptions m_exceptions;

    struct Object {
      ast::ObjectType type;
      ast::Text* source_text;
      std::shared_ptr<LocalDatabase> database;
    };
    
    std::unordered_map<std::string, std::unordered_map<std::string, Object>> a_map;

    void print(std::unordered_map<std::string, Object>& m);

    template<typename Func>
    bool traverseAll(std::string& library, std::string& package, Func func);
    template<typename Func>
    bool traverseLibrary(std::unordered_map<std::string, Object>& m, std::string library, std::string& package, Func func);
    
  public:
    GlobalDatabase() : m_debug(this) {};
    void append(std::shared_ptr<LocalDatabase>& d, std::string& library, std::string& object_name, ast::ObjectType type, ast::Text* source_text);
    bool findAll(DatabaseResults& results, const std::string& name, std::string& package, std::string& library);
    bool findObject(std::shared_ptr<LocalDatabase>& object, std::string& library, const std::string& name, ast::ObjectType type = ast::ObjectType::UNKNOWN);
    bool setVisible(std::string name = "", std::string package = "", std::string library = "");
    void print(std::string library = "");
  };

  template<typename Func>
  bool GlobalDatabase::traverseAll(std::string& library, std::string& package, Func func) {
    m_debug.functionStart("traverse(library = " + library + ", package = " + package + ")");
    bool found = false;
    if (library.empty()) {
      for (auto& i : a_map) {
        found |= traverseLibrary(i.second, i.first, package, func);
      }
    } else {
      auto i = a_map.find(library);
      if (i != a_map.end()) {
        found |= traverseLibrary(i->second, library, package, func);
      }
    }
    m_debug.functionEnd("traverseAll: " + std::to_string(found));
    return found;
  }
  
  template<typename Func>
  bool GlobalDatabase::traverseLibrary(std::unordered_map<std::string, Object>& m, std::string library, std::string& package, Func func) {
    m_debug.functionStart("traverseLibrary(library = " + library + ", package = " + package + ")");
    bool found = false;
    if (package.empty()) {
      for (auto& i : m) {
        func(i.second.database, library, i.first);
        found = true;
      }
    } else {
      auto i = m.find(package);
      if (i != m.end()) {
        func(i->second.database, library, package);
        found = true;
      }
    }
    m_debug.functionEnd("traverseLibrary: " + std::to_string(found));
    return found;
  }

}

#endif
