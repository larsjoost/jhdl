#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_type.hpp"
#include "range_direction.hpp"
#include "expression.hpp"


namespace vhdl {
  namespace parser {
  
    RangeType* RangeType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      left = scanner->expect<Expression>();
      rangeDirection = scanner->expect<RangeDirection>();
      right = scanner->expect<Expression>();
      return this;
    }

  }
}
