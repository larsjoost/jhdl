#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "simple_identifier.hpp"

namespace ast {
    
  class UseClause {

  public:
    SimpleIdentifier* library;
    SimpleIdentifier* package;
    SimpleIdentifier* identifier;
    
  };
}

#endif 
