
#ifndef VHDL_PARSER_CONSTANTDECLARATION_H_
#define VHDL_PARSER_CONSTANTDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/constant_declaration.hpp"
#include "basic_identifier.hpp"
#include "declaration_initialization.hpp"

namespace vhdl {
  namespace parser {

    template<bool defaultType = false>
    class ConstantDeclaration :
      public ::ast::ConstantDeclaration  {

    public:

      ConstantDeclaration<defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        if (defaultType) {
          scanner->optional(scanner::Scanner::VHDL_CONSTANT);
        } else {
          scanner->accept(scanner::Scanner::VHDL_CONSTANT);
        }
        identifier = scanner->expect<BasicIdentifier>();
        scanner->expect(":");
        type = scanner->expect<BasicIdentifier>();
        initialization = scanner->optional<DeclarationInitialization>();
        return this;
      }
      
    };

  }

}
  

#endif
