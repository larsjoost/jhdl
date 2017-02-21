
#ifndef AST_BLOCKSTATEMENT_H_
#define AST_BLOCKSTATEMENT_H_

#include "basic_identifier.hpp"
#include "list.hpp"
#include "concurrent_statement.hpp"
#include "declaration.hpp"

namespace ast {

  class ConcurrentStatement;
  
  class BlockStatement {
    
  public:
    
    BasicIdentifier* name;
    List<Declaration> declarations;
    List<ConcurrentStatement> concurrentStatements;
    
  };

}

#endif
