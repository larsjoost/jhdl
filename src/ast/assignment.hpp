
#ifndef AST_ASSIGNMENT_H_
#define AST_ASSIGNMENT_H_

#include "assignment_condition.hpp"
#include "list.hpp"

namespace ast {
    
  class Assignment {
    
  public:

    List<AssignmentCondition> assignment_conditions;
    
  };

}

#endif
