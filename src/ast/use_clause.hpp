#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "simple_identifier.hpp"
#include "string.hpp"

namespace ast {
    
  class UseClause {

  public:
    SimpleIdentifier* library = NULL;
    SimpleIdentifier* package = NULL;
    SimpleIdentifier* identifier = NULL;
    Text* all = NULL;
    String* string_identifier = NULL;
    
  };
}

#endif 
