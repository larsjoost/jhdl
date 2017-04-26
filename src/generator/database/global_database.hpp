#ifndef _GLOBAL_DATABASE_HPP_
#define _GLOBAL_DATABASE_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"
#include "local_database.hpp"

namespace generator {

  class GlobalDatabase {
    
    std::unordered_map<std::string, LocalDatabase> map;
  public:
    void append(std::string& name, LocalDatabase& d);
    void find(DatabaseResults& results, std::string& name, std::string package = "", std::string library = "");
    bool setVisible(std::string name = "", std::string package = "", std::string library = "");
    bool exists(std::string& name);
    void print(std::string name = "");
  };

  
}

#endif
