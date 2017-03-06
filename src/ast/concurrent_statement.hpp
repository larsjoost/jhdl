
#ifndef AST_CONCURRENT_STATEMENT_H_
#define AST_CONCURRENT_STATEMENT_H_

#include "method.hpp"
#include "forgenerate_statement.hpp"
#include "block_statement.hpp"
#include "signal_assignment.hpp"
#include "component_instance.hpp"

namespace ast {

  class ForGenerateStatement;
  class BlockStatement;
  
  class ConcurrentStatement {
    
  public:

    SignalAssignment* signalAssignment;
    Method* method;
    ForGenerateStatement* forGenerateStatement;
    BlockStatement* blockStatement;
    ComponentInstance* componentInstance;
    
  };

}

#endif
