#ifndef _DATABASE_GENERAL_HPP_
#define _DATABASE_GENERAL_HPP_

#include <unordered_map>
#include <memory>
#include <list>

#include "../../exceptions/exceptions.hpp"

#include "../../ast/object_type.hpp"
#include "../../ast/attribute.hpp"
#include "../../ast/function_declaration.hpp"
#include "../../ast/text.hpp"

namespace generator {

  struct DatabaseElement {
    ast::ObjectType id;
    std::string name;
    ast::ObjectArguments arguments;
    ast::ObjectValueContainer type;
    bool visible;
    ast::Attribute* attribute = NULL;
    ast::FunctionDeclaration* function;
    ast::Text* text;
    void print();
    std::string toString();
  };
  
  struct DatabaseResult {
    std::shared_ptr<std::list<std::string>> hierarchy;
    DatabaseElement* object = NULL;
    bool local;
    bool valid;
    std::string toString();
    void print();
    std::string hierarchyToString(std::string first_delimiter = "", std::string delimiter = ".");
    int hierarchySize();
  };

  using DatabaseResults = std::list<DatabaseResult>;

  struct ParentInfo {
    ast::ObjectType type;
    std::string name;
  };
  
}

#endif
