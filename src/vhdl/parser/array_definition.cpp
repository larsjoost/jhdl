#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "array_definition.hpp"
#include "array_subtype_definition.hpp"
#include "range_type.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    ArrayDefinition* ArrayDefinition::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (
       (range = scanner->optional<RangeType>()) ||
       (subtype = scanner->optional<ArraySubtypeDefinition>()) ||
       (identifier = scanner->optional<SimpleIdentifier>())
       );
      return this;
    }

  }
}
