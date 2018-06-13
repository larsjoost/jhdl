
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "element_declaration.hpp"
#include "simple_identifier.hpp"
#include "subtype_indication.hpp"

namespace vhdl {
  namespace parser {
  
    ElementDeclaration* ElementDeclaration::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifiers.add(scanner->accept<SimpleIdentifier>());
      while (scanner->optional(",")) {
        identifiers.add(scanner->expect<SimpleIdentifier, ElementDeclaration>());
      }
      scanner->expect(":");
      type = scanner->optional<SubtypeIndication>();
      return this;
    }

  }
}

