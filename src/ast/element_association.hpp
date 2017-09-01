
#ifndef AST_ELEMENTASSOCIATION_H_
#define AST_ELEMENTASSOCIATION_H_

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "choises.hpp"

namespace ast {

  class Choises;
  
  class ElementAssociation {
    
  public:
    
    Text text;

    Choises* choises;
    Expression* expression;
    
  };

}

#endif
