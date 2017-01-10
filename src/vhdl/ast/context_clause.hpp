
#ifndef VHDL_AST_CONTEXTCLAUSE_H_
#define VHDL_AST_CONTEXTCLAUSE_H_

#include "../../ast/list.hpp"
#include "use_clause.hpp"

namespace vhdl {
  namespace ast {

    class ContextClause {
    public:
      ::ast::List<UseClause> useClauses;
    };
    
  }
}

#endif 

