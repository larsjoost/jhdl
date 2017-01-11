
#ifndef VHDL_AST_DESIGNUNIT_HPP_
#define VHDL_AST_DESIGNUNIT_HPP_

#include "architecture.hpp"
#include "entity.hpp"
#include "context_clause.hpp"

namespace vhdl {
  namespace ast {
    
    class DesignUnit {
      
    public:
      ContextClause* contextClause = NULL;
      Entity* entity = NULL;
      Architecture* architecture = NULL;

    };

  }
}

#endif
