#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "entity.hpp"
#include "simple_identifier.hpp"
#include "interface_list.hpp"

namespace vhdl {
  namespace parser {
  
    Entity* Entity::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ENTITY);
      name = scanner->expect<SimpleIdentifier, Entity>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      if (scanner->optional(scanner::Scanner::VHDL_GENERIC)) {
        generics = scanner->optional<InterfaceList<scanner::Scanner::VHDL_CONSTANT>>();
        scanner->expect(";");
      }
      if (scanner->optional(scanner::Scanner::VHDL_PORT)) {
        ports = scanner->optional<InterfaceList<scanner::Scanner::VHDL_SIGNAL>>();
        scanner->expect(";");
      }
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_ENTITY);
      SimpleIdentifier* i = scanner->expect<SimpleIdentifier, Entity>();
      if (!name->equals(i)) {
        scanner->error("Identifier '" + i->toString() +
                       "' does not match entity name '" +
                       name->toString() + "'");
      }
      scanner->expect(";");
      return this;
    }

  }
}


