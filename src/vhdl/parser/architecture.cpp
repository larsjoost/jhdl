
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "architecture.hpp"
#include "simple_identifier.hpp"
#include "process.hpp"
#include "declaration.hpp"
#include "concurrent_statement.hpp"

namespace vhdl {
  namespace parser {
  
    Architecture* Architecture::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ARCHITECTURE);
      SimpleIdentifier* architectureName = scanner->expect<SimpleIdentifier, Architecture>();
      scanner->expect(scanner::Scanner::VHDL_OF);
      name = scanner->expect<SimpleIdentifier, Architecture>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {}
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (concurrentStatements.add(scanner->optional<ConcurrentStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE);
      SimpleIdentifier* i = scanner->expect<SimpleIdentifier, Architecture>();
      if (!architectureName->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match architecture name '" +
                       architectureName->toString() + "'");
      }
      scanner->expect(";");
      return this;
    }
    
  }
}

