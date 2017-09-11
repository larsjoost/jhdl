#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_direction_type.hpp"
#include "range_direction.hpp"
#include "expression.hpp"


namespace vhdl {
  namespace parser {
  
    RangeDirectionType* RangeDirectionType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      left = scanner->accept<Expression>();
      range_direction = scanner->accept<RangeDirection>();
      right = scanner->expect<Expression>();
      return this;
    }

  }
}
