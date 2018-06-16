
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "assignment.hpp"
#include "target.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    Target* target;
    bool new_operation;
    Assignment* assignment;
    
  };

}

#endif
