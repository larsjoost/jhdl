#include <cassert>
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "module.hpp"
#include "list.hpp"
#include "simple_identifier.hpp"

namespace verilog {
  namespace parser {
  
    Module* Module::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      m_debug.functionStart("parse");
      scanner->accept(scanner::Scanner::VERILOG_MODULE);
      interface = new ::ast::Interface();
      implementation = new ::ast::Implementation();
      SimpleIdentifier* name = scanner->expect<SimpleIdentifier, Module>(); 
      interface->name = name;
      implementation->entity_name = name;
      if (scanner->optional("(")) {
        auto i = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>, Module>();
        scanner->expect(")");
      }
      scanner->expect(";");
      scanner->expect(scanner::Scanner::VERILOG_ENDMODULE);
      m_debug.functionEnd("parse");
      return this;
    }

  }
}


