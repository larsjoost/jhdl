
#ifndef AST_ASSOCIATION_ELEMENT_H_
#define AST_ASSOCIATION_ELEMENT_H_

#include "expression.hpp"
#include "association_element_formal_part.hpp"

namespace ast {
    
  class Expression;

  class AssociationElement {
    
  public:

    AssociationElementFormalPart* formalPart;
    Expression* actualPart = NULL;
    
  };

}

#endif
