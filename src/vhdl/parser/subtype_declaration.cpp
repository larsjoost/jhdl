#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "subtype_declaration.hpp"
#include "simple_identifier.hpp"
#include "subtype_indication.hpp"

namespace vhdl {
  namespace parser {
  
    SubtypeDeclaration* SubtypeDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_SUBTYPE);
      identifier = scanner->expect<SimpleIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      if (scanner->LookAhead(0, ::ast::TOKEN_IDENTIFIER) &&
          scanner->LookAhead(1, ::ast::TOKEN_IDENTIFIER)) {
        resolution_function = scanner->expect<SimpleIdentifier>();
      }
      type = scanner->expect<SubtypeIndication>();
      return this;
    }

  }
}

