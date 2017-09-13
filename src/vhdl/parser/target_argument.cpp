#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "target_argument.hpp"
#include "expression.hpp"
#include "range_type.hpp"

namespace vhdl {
  namespace parser {
  
    TargetArgument* TargetArgument::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ((range = scanner->optional<RangeType>()) ||
       (index = scanner->optional<Expression>()));
      return this;
    }

  }
}
