
#ifndef VHDL_PARSER_SIGNALDECLARATION_H_
#define VHDL_PARSER_SIGNALDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "../../ast/signal_declaration.hpp"
#include "declaration_initialization.hpp"

namespace vhdl {
  namespace parser {

    template<bool defaultType = false>
    class SignalDeclaration :
      public ::ast::SignalDeclaration  {

    public:

      SignalDeclaration<defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        if (defaultType) {
          scanner->optional(scanner::Scanner::VHDL_SIGNAL);
        } else {
          scanner->accept(scanner::Scanner::VHDL_SIGNAL);
        }
        identifier = scanner->accept<BasicIdentifier>();
        scanner->expect(":");
        type = scanner->expect<BasicIdentifier>();
        initialization = scanner->optional<DeclarationInitialization>();
        return this;
      }
      
    };

  }

}
  

#endif
