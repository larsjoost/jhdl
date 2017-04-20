
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "simple_identifier.hpp"
#include "expression.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    SimpleIdentifier* identifier;
    Expression* expression;
    
  };

}

#endif
