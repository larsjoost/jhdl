
#ifndef AST_SIGNAL_ASSIGNMENT_CONDITION_HPP_
#define AST_SIGNAL_ASSIGNMENT_CONDITION_HPP_

#include "expression.hpp"

namespace ast {
    
  class SignalAssignmentCondition {
    
  public:

    Expression* expression;
    Expression* condition;
    
  };

}

#endif
