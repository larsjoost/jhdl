
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "architecture.hpp"
#include "basic_identifier.hpp"
#include "process.hpp"

namespace vhdl {
  namespace parser {
  
    Architecture* Architecture::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_ARCHITECTURE);
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      ::ast::Text architectureName = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_OF);
      scanner->skipOneOrMoreWhiteSpaces();
      i = scanner->expect<BasicIdentifier>();
      name = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_IS);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_BEGIN);
      scanner->skipOneOrMoreWhiteSpaces();
      parseBody(scanner);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->skipOneOrMoreWhiteSpaces();
      if (scanner->optional(scanner::Scanner::VHDL_ARCHITECTURE)) {
        scanner->skipOneOrMoreWhiteSpaces();
      };
      i = scanner->expect<BasicIdentifier>();
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
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

