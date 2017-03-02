
#ifndef AST_MODULE_H_
#define AST_MODULE_H_

#include "text.hpp"
#include "list.hpp"
#include "interface.hpp"
#include "implementation.hpp"
#include "context_clause.hpp"

namespace ast {
    
  class Module {
    
  public:
    
    ContextClause* contextClause = NULL;
    Interface* interface = NULL;
    Implementation* implementation = NULL;
    
  };

}

#endif
