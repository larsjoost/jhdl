
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "basic_identifier.hpp"
#include "expression.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    BasicIdentifier* identifier;
    Expression* expression;
    
  };

}

#endif
