#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "signal_assignment.hpp"
#include "assignment.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    SignalAssignment* SignalAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->lookAhead(":", 1)) {
        label = scanner->accept<SimpleIdentifier>();
        scanner->accept(":");
      }
      target = scanner->accept<BasicIdentifier>();
      scanner->accept("<=");
      assignment = scanner->expect<Assignment, SignalAssignment>();
      return this;
    }

  }
}
