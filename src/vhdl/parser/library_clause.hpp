#ifndef VHDL_PARSER_LIBRARYCLAUSE_H_
#define VHDL_PARSER_LIBRARYCLAUSE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"

namespace vhdl {
  namespace parser {
    
    class LibraryClause {
    
    public:

      LibraryClause* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
  }
}

#endif
