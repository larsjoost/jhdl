#ifndef _DATABASE_NAMEMAP_HPP_
#define _DATABASE_NAMEMAP_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"

namespace generator {

  class NameMap {
    std::string section;
    std::unordered_map<std::string, std::list<DatabaseElement>> map;
    void setVisible(std::list<DatabaseElement>& l);
  public:
    NameMap(std::string section) : section(section) {}
    bool setVisible(std::string name = "");
    void add(std::string& name, DatabaseElement& e);
    std::list<DatabaseElement>* find(std::string& name);
    void print();
    static void print(DatabaseElement* e);
    std::string getSection();
  };
  
}

#endif
