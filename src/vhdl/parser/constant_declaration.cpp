#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "constant_declaration.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ConstantDeclaration* ConstantDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_CONSTANT);
      identifier = scanner->expect<BasicIdentifier>();
      scanner->expect(":");
      type = scanner->expect<BasicIdentifier>();
      return this;
    }

  }
}
