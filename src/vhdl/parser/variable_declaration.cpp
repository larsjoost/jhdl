#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "variable_declaration.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    VariableDeclaration* VariableDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_VARIABLE);
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      identifier = i->text;
      scanner->skipWhiteSpace();
      scanner->expect(":");
      scanner->skipWhiteSpace();
      i = scanner->expect<BasicIdentifier>();
      type = i->text;
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}
