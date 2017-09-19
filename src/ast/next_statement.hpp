
#ifndef AST_NEXT_H_
#define AST_NEXT_H_

#include "simple_identifier.hpp"
#include "expression.hpp"

namespace ast {

  class NextStatement {
    
  public:
    
    SimpleIdentifier* label;
    SimpleIdentifier* loop_label = NULL;
    Expression* expression = NULL;
    
  };

}

#endif
