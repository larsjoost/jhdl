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

  struct SignalAssignmentContainer {
    ast::Text* text;
    int index;
  };
  
  struct DatabaseElement {
    ast::ObjectType id;
    std::string name;
    ast::ObjectInterface interface;
    ast::ObjectValueContainer type;
    bool visible = false;
    ast::Attribute* attribute = NULL;
    ast::FunctionDeclaration* function = NULL;
    std::unordered_map<std::string, SignalAssignmentContainer> signal_assignments;
    ast::Text* text = NULL;
    void print();
    std::string toString();
    int getSignalAssignmentIndex(std::string& name, ast::Text* text);
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
