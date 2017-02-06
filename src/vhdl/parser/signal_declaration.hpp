
#ifndef VHDL_PARSER_SIGNALDECLARATION_H_
#define VHDL_PARSER_SIGNALDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/signal_declaration.hpp"

namespace vhdl {
  namespace parser {

    class SignalDeclaration :
      public ::ast::SignalDeclaration  {

    public:

      SignalDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
