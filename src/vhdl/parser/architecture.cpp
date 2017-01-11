
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "architecture.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Architecture* Architecture::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      scanner->accept(scanner::VHDL_ARCHITECTURE);
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      architectureName = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VHDL_OF);
      scanner->skipOneOrMoreWhiteSpaces();
      i = scanner->expect<BasicIdentifier>();
      entityName = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VHDL_IS);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VHDL_BEGIN);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VHDL_END);
      scanner->skipOneOrMoreWhiteSpaces();
      if (scanner->optional(scanner::VHDL_ARCHITECTURE)) {
        scanner->skipOneOrMoreWhiteSpaces();
      };
      scanner->expect(architectureName);
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }
    
  }
}

