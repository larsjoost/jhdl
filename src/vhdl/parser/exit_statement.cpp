#include "../../ast/scanner.hpp"
#include "../../ast/conditional_statement.hpp"
#include "../scanner/scanner.hpp"
#include "exit_statement.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ExitStatement* ExitStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->lookAhead(":", 1)) {
        label = scanner->accept<SimpleIdentifier>();
        scanner->accept(":");
      }
      scanner->accept(scanner::Scanner::VHDL_EXIT);
      loop_label = scanner->optional<SimpleIdentifier>();
      if (scanner->optional(scanner::Scanner::VHDL_WHEN)) {
        expression = scanner->expect<Expression, ExitStatement>();
      }
      return this;
    }

  }
}
