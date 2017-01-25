#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "type_declaration.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    TypeDeclaration* TypeDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_TYPE);
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      identifier = i->text;
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_IS);
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_RANGE);
      left = scanner->expect<Expression>();
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_TO);
      right = scanner->expect<Expression>();
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}

