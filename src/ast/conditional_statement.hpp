
#ifndef AST_CONDITIONAL_STATEMENT_H_
#define AST_CONDITIONAL_STATEMENT_H_

#include "expression.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"

namespace ast {

  class SequentialStatement;
  
  class ConditionalStatement {
    
  public:

    Expression* condition;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
