
#ifndef AST_ASSOCIATION_ELEMENT_H_
#define AST_ASSOCIATION_ELEMENT_H_

#include "expression.hpp"

namespace ast {
    
  class Expression;

  class AssociationElement {
    
  public:
    
    Expression* expression = NULL;
    
  };

}

#endif
