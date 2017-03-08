#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "subtype_declaration.hpp"
#include "basic_identifier.hpp"
#include "subtype_indication.hpp"

namespace vhdl {
  namespace parser {
  
    SubtypeDeclaration* SubtypeDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_SUBTYPE);
      identifier = scanner->expect<BasicIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      type = scanner->expect<SubtypeIndication>();
      return this;
    }

  }
}

