#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_type.hpp"
#include "range_direction_type.hpp"
#include "range_attribute_type.hpp"
#include "expression.hpp"


namespace vhdl {
  namespace parser {
  
    RangeType* RangeType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (
       (range_attribute_type = scanner->optional<RangeAttributeType>()) ||
       (range_direction_type = scanner->optional<RangeDirectionType>()));
      return this;
    }

  }
}
