
#ifndef VHDL_PARSER_VARIABLEDECLARATION_H_
#define VHDL_PARSER_VARIABLEDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/variable_declaration.hpp"

namespace vhdl {
  namespace parser {

    class VariableDeclaration :
      public ::ast::VariableDeclaration  {

    public:

      VariableDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
