#include <cassert>
#include "../scanner/defines.hpp"
#include "../scanner/scanner.hpp"
#include "module.hpp"
#include "basic_identifier.hpp"
#include "basic_identifier_list.hpp"

namespace verilog {
  namespace parser {
  
    Module* Module::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      scanner->accept(scanner::VERILOG_MODULE);
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      entityName = i->text;
      scanner->skipWhiteSpace();
      if (scanner->optional("(")) {
        BasicIdentifierList* i;
        i = scanner->expect<BasicIdentifierList>();
        port = i->textList;
        scanner->skipWhiteSpace();
        scanner->expect(")");
      }
      scanner->skipWhiteSpace();
      scanner->expect(";");
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VERILOG_ENDMODULE);
      return this;
    }

  }
}


