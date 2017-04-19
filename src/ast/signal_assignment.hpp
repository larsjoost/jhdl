
#ifndef AST_SIGNAL_ASSIGNMENT_H_
#define AST_SIGNAL_ASSIGNMENT_H_

#include "signal_assignment_condition.hpp"
#include "basic_identifier.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "list.hpp"

namespace ast {
    
  class SignalAssignment {
    
  public:

    std::string name = "";
    SimpleIdentifier* label;
    SimpleIdentifier* identifier;
    List<SignalAssignmentCondition> signalAssignmentConditions;
    
  };

}

#endif
