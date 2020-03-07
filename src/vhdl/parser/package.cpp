
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "package.hpp"
#include "new_package_declaration.hpp"
#include "simple_identifier.hpp"
#include "declaration.hpp"
#include "interface_list.hpp"

namespace vhdl {
  namespace parser {
  
    Package* Package::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      m_debug.functionStart("parse");
      scanner->accept(scanner::Scanner::VHDL_PACKAGE);
      body = scanner->optional(scanner::Scanner::VHDL_BODY);
      name = scanner->expect<SimpleIdentifier, Package>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      if (scanner->optional(scanner::Scanner::VHDL_NEW)) {
        new_package_declaration = scanner->expect<NewPackageDeclaration, Package>();
      } else {
        if (scanner->optional(scanner::Scanner::VHDL_GENERIC)) {
          generics = scanner->optional<InterfaceList<scanner::Scanner::VHDL_CONSTANT>>();
          scanner->expect(";");
        }
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
      }
      m_debug.functionEnd("parse");
      return this;
    }
    
  }
}

