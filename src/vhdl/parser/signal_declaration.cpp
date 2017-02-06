#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "signal_declaration.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    SignalDeclaration* SignalDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_SIGNAL);
      identifier = scanner->expect<BasicIdentifier>();
      scanner->expect(":");
      type = scanner->expect<BasicIdentifier>();
      scanner->expect(";");
      return this;
    }

  }
}
