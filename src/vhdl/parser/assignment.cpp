#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/assignment_condition.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "assignment.hpp"

namespace vhdl {
  namespace parser {
  
    Assignment* Assignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      do {
        ::ast::AssignmentCondition s;
        s.expression = scanner->expect<Expression, Assignment>();
        if (scanner->optional(scanner::Scanner::VHDL_WHEN)) {
          s.condition = scanner->expect<Expression, Assignment>();
        } else {
          s.condition = NULL;
          assignment_conditions.add(&s);
          break;
        }
        assignment_conditions.add(&s);
      } while (scanner->optional(scanner::Scanner::VHDL_ELSE));
      return this;
    }

  }
}
