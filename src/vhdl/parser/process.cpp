

#include "../../ast/scanner.hpp"
#include "../../ast/sequential_statement.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "process.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"
#include "list.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Process* Process::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->lookAhead(":", 1)) {
        label = scanner->accept<SimpleIdentifier>();
        scanner->accept(":");
      }
      scanner->accept(scanner::Scanner::VHDL_PROCESS);
      if (scanner->optional("(")) {
        sensitivity = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>>();
        scanner->expect(")");
      }
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {};
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_PROCESS);
      return this;
    }
   
  }
}

