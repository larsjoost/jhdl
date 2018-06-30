
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "assignment.hpp"
#include "basic_identifier.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    BasicIdentifier* target;
    bool new_operation;
    Assignment* assignment;
    
  };

}

#endif
