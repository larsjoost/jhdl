#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_type.hpp"
#include "subtype_indication.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    SubtypeIndication* SubtypeIndication::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      name = scanner->accept<SimpleIdentifier>();
      if (scanner->optional(scanner::Scanner::VHDL_RANGE)) {
        range = scanner->expect<RangeType>();
      } else if (scanner->optional("(")) {
        range = scanner->expect<RangeType>();
        scanner->expect(")");
      }
      return this;
    }

  }
}
