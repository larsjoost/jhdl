
#ifndef AST_DESIGNUNIT_HPP_
#define AST_DESIGNUNIT_HPP_

#include "architecture.hpp"
#include "entity.hpp"
#include "context_clause.hpp"

namespace ast {
    
  class DesignUnit {
      
  public:
    ContextClause* contextClause = NULL;
    Entity* entity = NULL;
    Architecture* architecture = NULL;

  };

}

#endif
