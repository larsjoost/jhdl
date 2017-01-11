#ifndef VHDL_PARSER_USECLAUSE_H_
#define VHDL_PARSER_USECLAUSE_H_

#include "../scanner/scanner.hpp"
#include "../ast/use_clause.hpp"

namespace vhdl {
  namespace parser {
    
    class UseClause :
      public ast::UseClause {
    
    public:

      UseClause* parse(scanner::Scanner* s);

    };
  }
}

#endif
