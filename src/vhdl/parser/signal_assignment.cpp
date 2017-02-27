#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "signal_assignment.hpp"

namespace vhdl {
  namespace parser {
  
    SignalAssignment* SignalAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<BasicIdentifier>();
      scanner->accept("<=");
      expression = scanner->expect<Expression>();
      return this;
    }

  }
}
