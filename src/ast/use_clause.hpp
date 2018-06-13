#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "simple_identifier.hpp"
#include "string.hpp"

namespace ast {
    
  class UseClause {

  public:
    SimpleIdentifier* library;
    SimpleIdentifier* package;
    SimpleIdentifier* identifier;
    String* string_identifier;
    
  };
}

#endif 
