
#ifndef AST_FORGENERATE_H_
#define AST_FORGENERATE_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "concurrent_statement.hpp"
#include "range_type.hpp"
#include "declaration.hpp"

namespace ast {

  class ConcurrentStatement;
  
  class ForGenerateStatement {
    
  public:

    SimpleIdentifier* name;
    SimpleIdentifier* identifier;
    RangeType* range;
    List<Declaration> declarations;
    List<ConcurrentStatement> concurrentStatements;
    
  };

}

#endif
