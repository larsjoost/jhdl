
#ifndef AST_CONCURRENT_STATEMENT_H_
#define AST_CONCURRENT_STATEMENT_H_

#include "method.hpp"
#include "forgenerate_statement.hpp"

namespace ast {

  class ForGenerateStatement;
  
  class ConcurrentStatement {
    
  public:

    Method* method;
    ForGenerateStatement* forGenerateStatement;

  };

}

#endif
