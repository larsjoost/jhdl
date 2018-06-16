
#ifndef AST_ASSIGNMENT_CONDITION_HPP_
#define AST_ASSIGNMENT_CONDITION_HPP_

#include "expression.hpp"

namespace ast {
    
  class AssignmentCondition {
    
  public:

    Expression* expression;
    Expression* condition;
    
  };

}

#endif
