
#ifndef VHDL_PARSER_CONTEXTCLAUSE_H_
#define VHDL_PARSER_CONTEXTCLAUSE_H_

#include "../scanner/scanner.hpp"
#include "../ast/context_clause.hpp"

namespace vhdl {
  namespace parser {
    
    class ContextClause  :
      public ast::ContextClause {
	
    public:

      void parse(scanner::Scanner* s);

    };
    
  }
}

#endif

