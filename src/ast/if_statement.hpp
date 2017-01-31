
#ifndef AST_IF_STATEMENT_H_
#define AST_IF_STATEMENT_H_

#include "conditional_statement.hpp"

namespace ast {
    
  class ConditionalStatement;

  class IfStatement {
    
  public:

    List<ConditionalStatement> conditionalStatements;
    
  };

}

#endif
