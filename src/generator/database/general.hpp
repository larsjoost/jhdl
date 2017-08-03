#ifndef _DATABASE_GENERAL_HPP_
#define _DATABASE_GENERAL_HPP_

#include <unordered_map>
#include <list>

#include "../../exceptions/exceptions.hpp"

#include "../../ast/object_type.hpp"
#include "../../ast/attribute.hpp"
#include "../../ast/function_declaration.hpp"
#include "../../ast/text.hpp"

namespace generator {

  struct DatabaseElement {
    ast::ObjectType id;
    std::string library;
    ast::ObjectType sectionType;
    std::string sectionName;
    std::string name;
    ast::ObjectArguments arguments;
    ast::ObjectValueContainer type;
    bool visible;
    ast::Attribute* attribute = NULL;
    ast::FunctionDeclaration* function;
    ast::Text* text;
    int hierarchyLevel = 0;
    void print();
    std::string toString();
  };
  
  struct DatabaseResult {
    bool local;
    DatabaseElement* object;
    std::string toString();
  };

  using DatabaseResults = std::list<DatabaseResult>;
  
}

#endif
