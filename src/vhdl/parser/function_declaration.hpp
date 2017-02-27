
#ifndef VHDL_PARSER_FUNCTIONDECLARATION_H_
#define VHDL_PARSER_FUNCTIONDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/function_declaration.hpp"

namespace vhdl {
  namespace parser {

    class FunctionDeclaration :
      public ::ast::FunctionDeclaration  {

    public:

      FunctionDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
