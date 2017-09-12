
#ifndef AST_SIGNAL_ASSIGNMENT_H_
#define AST_SIGNAL_ASSIGNMENT_H_

#include "signal_assignment_condition.hpp"
#include "target.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "list.hpp"

namespace ast {
    
  class SignalAssignment {
    
  public:

    std::string name = "";
    SimpleIdentifier* label;
    Target* target;
    List<SignalAssignmentCondition> signalAssignmentConditions;
    
  };

}

#endif
