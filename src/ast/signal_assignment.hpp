
#ifndef AST_SIGNAL_ASSIGNMENT_H_
#define AST_SIGNAL_ASSIGNMENT_H_

#include "assignment.hpp"
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
    Assignment* assignment;
    
  };

}

#endif
