
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
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_ARCHITECTURE);
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      ::ast::Text architectureName = i->text;
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_OF);
      scanner->skipWhiteSpace();
      i = scanner->expect<BasicIdentifier>();
      name = i->text;
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_IS);
      parseDeclarations(scanner);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      parseBody(scanner);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->skipWhiteSpace();
      scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE);
      scanner->skipWhiteSpace();
      i = scanner->expect<BasicIdentifier>();
      if (!architectureName.equals(i->text)) {
        scanner->error("Identifier '" + i->text.toString() +
                       "' does not match architecture name '" +
                       architectureName.toString() + "'");
      }
      scanner->skipWhiteSpace();
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

