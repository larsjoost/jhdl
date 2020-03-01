
#ifndef AST_SIGNAL_ASSIGNMENT_H_
#define AST_SIGNAL_ASSIGNMENT_H_

#include "assignment.hpp"
#include "basic_identifier.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "list.hpp"

namespace ast {
    
  class SignalAssignment {
    
  public:

    SimpleIdentifier* label;
    BasicIdentifier* target;
    Assignment* assignment;
    
  };

}

#endif
