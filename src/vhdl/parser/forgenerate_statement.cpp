#include "../../ast/scanner.hpp"
#include "../../ast/conditional_statement.hpp"
#include "../scanner/scanner.hpp"
#include "forgenerate_statement.hpp"
#include "simple_identifier.hpp"
#include "concurrent_statement.hpp"
#include "iteration_scheme.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    ForGenerateStatement* ForGenerateStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      name = scanner->accept<SimpleIdentifier>();
      scanner->accept(":");
      scanner->accept(scanner::Scanner::VHDL_FOR);
      identifier = scanner->expect<SimpleIdentifier, ForGenerateStatement>();
      scanner->expect(scanner::Scanner::VHDL_IN);
      iteration = scanner->expect<IterationScheme, ForGenerateStatement>();
      scanner->expect(scanner::Scanner::VHDL_GENERATE); 
      if (declarations.add(scanner->optional<Declaration>())) {
        while (declarations.add(scanner->optional<Declaration>())) {};
        scanner->expect(scanner::Scanner::VHDL_BEGIN);
      }
      while (concurrentStatements.add(scanner->optional<ConcurrentStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_GENERATE);
      SimpleIdentifier* i = scanner->expect<SimpleIdentifier, ForGenerateStatement>();
      if (!name->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match generate name '" +
                       name->toString() + "'");
      }
      return this;
    }

  }
}
