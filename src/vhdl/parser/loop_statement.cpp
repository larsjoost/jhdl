#include "../../ast/scanner.hpp"
#include "../../ast/conditional_statement.hpp"
#include "../scanner/scanner.hpp"
#include "loop_statement.hpp"
#include "simple_identifier.hpp"
#include "sequential_statement.hpp"
#include "iteration_scheme.hpp"

namespace vhdl {
  namespace parser {
  
    LoopStatement* LoopStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->lookAhead(":", 1)) {
        label = scanner->accept<SimpleIdentifier>();
        scanner->accept(":");
      }
      scanner->accept(scanner::Scanner::VHDL_LOOP);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_LOOP);
      SimpleIdentifier* i;
      if (i = scanner->optional<SimpleIdentifier>()) {
        if (!label->equals(i)) {
          scanner->error("Identifier '" + i->toString() +
                         "' does not match loop name '" +
                         label->toString() + "'");
        }
      }
      return this;
    }

  }
}
