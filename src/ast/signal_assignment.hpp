
#ifndef AST_SIGNAL_ASSIGNMENT_H_
#define AST_SIGNAL_ASSIGNMENT_H_

#include "basic_identifier.hpp"
#include "expression.hpp"

namespace ast {
    
  class SignalAssignment {
    
  public:

    BasicIdentifier* identifier;
    Expression* expression;
    
  };

}

#endif