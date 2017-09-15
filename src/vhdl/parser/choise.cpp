
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "choise.hpp"
#include "range_direction_type.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    Choise* Choise::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (
       (others = scanner->optional(scanner::Scanner::VHDL_OTHERS)) ||
       (discrete_range = scanner->optional<RangeDirectionType>()) ||
       (expression = scanner->optional<Expression>()));
      return this;
    }

  }
}

