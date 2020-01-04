
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
      architecture_name = scanner->expect<SimpleIdentifier, Architecture>();
      scanner->expect(scanner::Scanner::VHDL_OF);
      entity_name = scanner->expect<SimpleIdentifier, Architecture>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {}
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (concurrentStatements.add(scanner->optional<ConcurrentStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE);
      SimpleIdentifier* i = scanner->expect<SimpleIdentifier, Architecture>();
      if (!architecture_name->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match architecture name '" +
                       architecture_name->toString() + "'");
      }
      scanner->expect(";");
      return this;
    }
    
  }
}

