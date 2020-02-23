
#ifndef AST_CONCURRENT_STATEMENT_H_
#define AST_CONCURRENT_STATEMENT_H_

#include "process.hpp"
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
    Process* process;
    ForGenerateStatement* forGenerateStatement;
    BlockStatement* blockStatement;
    ComponentInstance* componentInstance;
    
  };

}

#endif
