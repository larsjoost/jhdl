
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "declaration_initialization.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {

    DeclarationInitialization* DeclarationInitialization::parse(::ast::Scanner<scanner::Scanner>* scanner) {
        scanner->accept(":=");
        value = scanner->expect<Expression, DeclarationInitialization>();
        return this;
    }

  }

}
  
