
#ifndef AST_VARIABLE_ASSIGNMENT_H_
#define AST_VARIABLE_ASSIGNMENT_H_

#include "simple_identifier.hpp"
#include "expression.hpp"
#include "association_list.hpp"

namespace ast {
    
  class VariableAssignment {
    
  public:

    SimpleIdentifier* identifier;
    AssociationList* arguments = NULL; 
    Expression* expression;
    
  };

}

#endif
