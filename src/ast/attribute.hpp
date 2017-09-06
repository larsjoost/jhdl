
#ifndef AST_ATTRIBUTE_H_
#define AST_ATTRIBUTE_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "string.hpp"
#include "object_type.hpp"
#include "string.hpp"
#include "scalar_type.hpp"
#include "return_statement.hpp"

namespace ast {
    
  class Attribute {
    
  public:

    SimpleIdentifier* identifier;
    SimpleIdentifier* item;
    String* string;
    List<SimpleIdentifier>* arguments;
    ReturnStatement* returnStatement;
    ObjectType objectType;
    SimpleIdentifier* typeMark;
    ScalarType* expression;

    std::string toString(bool setCase = false) {
      return identifier->toString(setCase);
    }
    
  };

}

#endif
