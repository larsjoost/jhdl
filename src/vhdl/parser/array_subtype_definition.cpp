#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "array_subtype_definition.hpp"
#include "simple_identifier.hpp"
#include "range_type.hpp"

namespace vhdl {
  namespace parser {
  
    ArraySubtypeDefinition* ArraySubtypeDefinition::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<SimpleIdentifier>();
      scanner->accept(scanner::Scanner::VHDL_RANGE);
      if (scanner->optional("<")) {
        scanner->expect(">");
      } else {
        range = scanner->expect<RangeType, ArraySubtypeDefinition>();
      }
      return this;
    }

  }
}
