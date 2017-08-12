#include <cassert>
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "module.hpp"
#include "list.hpp"
#include "simple_identifier.hpp"

namespace verilog {
  namespace parser {
  
    Module* Module::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      debug.functionStart("parse");
      scanner->accept(scanner::Scanner::VERILOG_MODULE);
      interface = new ::ast::Interface();
      implementation = new ::ast::Implementation();
      SimpleIdentifier* name = scanner->expect<SimpleIdentifier>(); 
      interface->name = name;
      implementation->name = name;
      if (scanner->optional("(")) {
        auto i = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>>();
        scanner->expect(")");
      }
      scanner->expect(";");
      scanner->expect(scanner::Scanner::VERILOG_ENDMODULE);
      debug.functionEnd("parse");
      return this;
    }

  }
}


