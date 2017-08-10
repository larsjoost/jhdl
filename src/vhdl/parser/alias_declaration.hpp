
#ifndef VHDL_PARSER_ALIASDECLARATION_H_
#define VHDL_PARSER_ALIASDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/alias_declaration.hpp"

namespace vhdl {
  namespace parser {

    class AliasDeclaration :
      public ::ast::AliasDeclaration  {

    public:

      AliasDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
