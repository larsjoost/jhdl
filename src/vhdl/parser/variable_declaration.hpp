
#ifndef VHDL_PARSER_VARIABLEDECLARATION_H_
#define VHDL_PARSER_VARIABLEDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "../../ast/variable_declaration.hpp"

namespace vhdl {
  namespace parser {

    template<bool defaultType = false>
    class VariableDeclaration :
      public ::ast::VariableDeclaration  {

    public:

      VariableDeclaration<defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        if (defaultType) {
          scanner->optional(scanner::Scanner::VHDL_VARIABLE);
        } else {
          scanner->accept(scanner::Scanner::VHDL_VARIABLE);
        }
        identifier = scanner->expect<BasicIdentifier>();
        scanner->expect(":");
        type = scanner->expect<BasicIdentifier>();
        return this;
      }

    };

  }

}
  

#endif
