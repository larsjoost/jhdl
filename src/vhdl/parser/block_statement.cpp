

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "block_statement.hpp"
#include "concurrent_statement.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    BlockStatement* BlockStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      name = scanner->accept<BasicIdentifier>();
      scanner->accept(":");
      scanner->accept(scanner::Scanner::VHDL_BLOCK);
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {};
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      while (concurrentStatements.add(scanner->optional<ConcurrentStatement>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_BLOCK);
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      if (!name->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match block name '" +
                       name->toString() + "'");
      }
      return this;
    }
   
  }
}
