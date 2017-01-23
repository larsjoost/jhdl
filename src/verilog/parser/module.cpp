#include <cassert>
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "module.hpp"
#include "basic_identifier.hpp"
#include "basic_identifier_list.hpp"

namespace verilog {
  namespace parser {
  
    Module* Module::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VERILOG_MODULE);
      interface = new ::ast::Interface();
      implementation = new ::ast::Implementation();
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      interface->name = i->text;
      scanner->skipWhiteSpace();
      if (scanner->optional("(")) {
        BasicIdentifierList* i = scanner->expect<BasicIdentifierList>();
        scanner->skipWhiteSpace();
        scanner->expect(")");
      }
      scanner->skipWhiteSpace();
      scanner->expect(";");
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VERILOG_ENDMODULE);
      return this;
    }

  }
}


