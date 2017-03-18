
#ifndef AST_FORLOOP_H_
#define AST_FORLOOP_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "range_type.hpp"

namespace ast {

  class SequentialStatement;
  
  class ForLoopStatement {
    
  public:
    
    SimpleIdentifier* identifier;
    RangeType* range;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
