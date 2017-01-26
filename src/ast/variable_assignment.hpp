
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "text.hpp"
#include "expression.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    Text identifier;
    Expression* expression;
    
  };

}

#endif
