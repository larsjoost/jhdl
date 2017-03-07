#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/signal_assignment_condition.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "signal_assignment.hpp"

namespace vhdl {
  namespace parser {
  
    SignalAssignment* SignalAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->lookAhead(":", 1)) {
        label = scanner->accept<BasicIdentifier>();
        scanner->accept(":");
      }
      identifier = scanner->accept<BasicIdentifier>();
      scanner->accept("<=");
      do {
        ::ast::SignalAssignmentCondition s;
        s.expression = scanner->expect<Expression>();
        if (scanner->optional(scanner::Scanner::VHDL_WHEN)) {
          s.condition = scanner->expect<Expression>();
        } else {
          s.condition = NULL;
          signalAssignmentConditions.add(&s);
          break;
        }
        signalAssignmentConditions.add(&s);
      } while (scanner->optional(scanner::Scanner::VHDL_ELSE));
      return this;
    }

  }
}
