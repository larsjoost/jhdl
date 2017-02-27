
#ifndef AST_RETURN_STATEMENT_H_
#define AST_RETURN_STATEMENT_H_

#include "expression.hpp"

namespace ast {
    
  class ReturnStatement {
    
  public:

    Expression* value;
    
  };

}

#endif
