
#ifndef VHDL_PARSER_DECLARATION_H_
#define VHDL_PARSER_DECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/declaration.hpp"

namespace vhdl {
  namespace parser {

    class Declaration :
      public ::ast::Declaration  {

    public:

      Declaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
