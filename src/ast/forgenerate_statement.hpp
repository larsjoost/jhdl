
#ifndef AST_FORGENERATE_H_
#define AST_FORGENERATE_H_

#include "basic_identifier.hpp"
#include "list.hpp"
#include "concurrent_statement.hpp"
#include "range_type.hpp"

namespace ast {

  class ConcurrentStatement;
  
  class ForGenerateStatement {
    
  public:

    BasicIdentifier* name;
    BasicIdentifier* identifier;
    RangeType* range;
    List<ConcurrentStatement> concurrentStatements;
    
  };

}

#endif
