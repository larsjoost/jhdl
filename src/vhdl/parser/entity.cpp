#include <cassert>
#include "../../ast/scanner.hpp"
#include "../scanner/defines.hpp"
#include "../scanner/scanner.hpp"
#include "entity.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Entity* Entity::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_ENTITY);
      name = scanner->expect<BasicIdentifier>();
      scanner->expect(scanner::Scanner::VHDL_IS);
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->optional(scanner::Scanner::VHDL_ENTITY);
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
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


