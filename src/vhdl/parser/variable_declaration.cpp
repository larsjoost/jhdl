#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "variable_declaration.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    VariableDeclaration* VariableDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_VARIABLE);
      identifier = scanner->expect<BasicIdentifier>();
      scanner->expect(":");
      type = scanner->expect<BasicIdentifier>();
      scanner->expect(";");
      return this;
    }

  }
}
