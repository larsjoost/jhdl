
#ifndef AST_IMPLEMENTATION_H_
#define AST_IMPLEMENTATION_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "declaration.hpp"
#include "concurrent_statement.hpp"

namespace ast {
    
  class Implementation {
    
  public:

    SimpleIdentifier* name;
    List<Declaration> declarations;
    List<ConcurrentStatement> concurrentStatements;
    
  };

}

#endif
