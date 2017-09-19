
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "target.hpp"
#include "expression.hpp"
#include "association_list.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    Target* target;
    bool new_operation;
    Expression* expression;
    
  };

}

#endif
