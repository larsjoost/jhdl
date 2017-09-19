
#ifndef AST_LOOP_H_
#define AST_LOOP_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "iteration_scheme.hpp"

namespace ast {

  class SequentialStatement;
  
  class LoopStatement {
    
  public:
    
    SimpleIdentifier* label;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
