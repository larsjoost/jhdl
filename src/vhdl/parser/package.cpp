
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "package.hpp"
#include "simple_identifier.hpp"
#include "declaration.hpp"

namespace vhdl {
  namespace parser {
  
    Package* Package::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_PACKAGE);
      body = scanner->optional(scanner::Scanner::VHDL_BODY);
      name = scanner->expect<SimpleIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (declarations.add(scanner->optional<Declaration>())) {}
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_PACKAGE);
      bool delimiterBody = scanner->optional(scanner::Scanner::VHDL_BODY);
      if (delimiterBody && !body) {
        scanner->error("Found package body termination, but not a body declaration");
      }
      SimpleIdentifier* i = scanner->optional<SimpleIdentifier>();
      if (i && !name->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match package name '" +
                       name->toString() + "'");
      }
      scanner->expect(";");
      return this;
    }
    
  }
}

