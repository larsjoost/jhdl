
#ifndef AST_WHILELOOP_H_
#define AST_WHILELOOP_H_

#include "expression.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"

namespace ast {

  class SequentialStatement;
  
  class WhileLoopStatement {
    
  public:
    
    Expression* condition;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
