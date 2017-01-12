
#ifndef AST_CONTEXTCLAUSE_H_
#define AST_CONTEXTCLAUSE_H_

#include "list.hpp"
#include "use_clause.hpp"

namespace ast {

  class ContextClause {
  public:
    List<UseClause> useClauses;
  };
  
}

#endif 

