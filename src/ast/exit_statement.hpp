
#ifndef AST_EXIT_H_
#define AST_EXIT_H_

#include "simple_identifier.hpp"
#include "expression.hpp"

namespace ast {

  class ExitStatement {
    
  public:
    
    SimpleIdentifier* label;
    SimpleIdentifier* loop_label = NULL;
    Expression* expression = NULL;
    
  };

}

#endif
