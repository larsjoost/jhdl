
#ifndef VHDL_PARSER_CONTEXTCLAUSE_H_
#define VHDL_PARSER_CONTEXTCLAUSE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/context_clause.hpp"

namespace vhdl {
  namespace parser {
    
    class ContextClause  :
      public ::ast::ContextClause {
	
    public:

      ContextClause* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

