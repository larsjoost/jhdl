#ifndef _DATABASE_GENERAL_HPP_
#define _DATABASE_GENERAL_HPP_

#include <unordered_map>
#include <list>

#include "../../exceptions/exceptions.hpp"

#include "../../ast/object_type.hpp"
#include "../../ast/attribute.hpp"
#include "../../ast/function_declaration.hpp"
#include "../../ast/procedure_declaration.hpp"
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
    ast::Attribute* attribute;
    ast::FunctionDeclaration* function;
    ast::ProcedureDeclaration* procedure;
    ast::Text* text;
    int hierarchyLevel = 0;
    void print();
    std::string toString();
  };
  
  struct DatabaseResult {
    bool local;
    DatabaseElement* object;
    std::string getName(bool fullName, int hierarchyLevel, std::string library = "", std::string package = "");
    std::string toString();
  };

  using DatabaseResults = std::list<DatabaseResult>;
  
}

#endif
