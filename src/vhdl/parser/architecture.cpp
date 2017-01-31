
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "architecture.hpp"
#include "basic_identifier.hpp"
#include "process.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Architecture* Architecture::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ARCHITECTURE);
      BasicIdentifier* architectureName = scanner->expect<BasicIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_OF);
      name = scanner->expect<BasicIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      parseDeclarations(scanner);
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      parseBody(scanner);
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE);
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      if (!architectureName->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match architecture name '" +
                       architectureName->toString() + "'");
      }
      scanner->expect(";");
      return this;
    }

    void Architecture::parseDeclarations(::ast::Scanner<scanner::Scanner>* scanner) {
      bool match;
      do {
        match = false;
        match |= declarations.add(scanner->optional<Declaration>());
      } while (match);
    }
    
    void Architecture::parseBody(::ast::Scanner<scanner::Scanner>* scanner) {
      bool match;
      do {
        match = false;
        match |= methods.add(scanner->optional<Process>());
      } while (match);
    }
    
  }
}

