#ifndef _DATABASE_NAMEMAP_HPP_
#define _DATABASE_NAMEMAP_HPP_

#include <unordered_map>
#include <list>

#include "general.hpp"

namespace generator {

  class NameMap {
    std::string a_section;
    ast::ObjectType a_type;
    void setVisible(std::list<DatabaseElement>& l);
  protected:
    std::unordered_map<std::string, std::list<DatabaseElement>> a_map;
  public:
    NameMap(std::string section, ast::ObjectType type) {a_section = section; a_type = type; }
    bool setVisible(std::string name = "");
    void add(std::string& name, DatabaseElement& e);
    void add(NameMap& other);
    std::list<DatabaseElement>* find(const std::string& name);
    void print();
    static void print(DatabaseElement* e);
    std::string GetSection() { return a_section; }
    ast::ObjectType GetType() { return a_type; }
  };
  
}

#endif
