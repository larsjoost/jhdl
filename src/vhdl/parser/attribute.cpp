#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "attribute.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Attribute* Attribute::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ATTRIBUTE);
      identifier = scanner->expect<SimpleIdentifier>();
      scanner->expect(":");
      typeMark = scanner->expect<SimpleIdentifier>();
      return this;
    }

  }
}

