#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "target.hpp"
#include "expression.hpp"
#include "range_type.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Target* Target::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<SimpleIdentifier>();
      if (scanner->optional("(")) {
        ((range = scanner->optional<RangeType>()) ||
         (index = scanner->optional<Expression>()));
        scanner->expect(")");
      }
      return this;
    }

  }
}
