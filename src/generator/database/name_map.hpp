#ifndef _DATABASE_NAMEMAP_HPP_
#define _DATABASE_NAMEMAP_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"

namespace generator {

  class NameMap {
    void setVisible(std::list<DatabaseElement>& l);
  protected:
    std::unordered_map<std::string, std::list<DatabaseElement>> a_map;
  public:
    bool setVisible(std::string name = "");
    void add(std::string& name, DatabaseElement& e);
    void add(NameMap& other);
    std::list<DatabaseElement>* find(const std::string& name);
    void print();
    static void print(DatabaseElement* e);
  };
  
}

#endif
