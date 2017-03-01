
#ifndef VHDL_PARSER_DECLARATIONINITIALIZATION_H_
#define VHDL_PARSER_DECLARATIONINITIALIZATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/declaration_initialization.hpp"

namespace vhdl {
  namespace parser {

    class DeclarationInitialization :
      public ::ast::DeclarationInitialization  {

    public:

      DeclarationInitialization* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
