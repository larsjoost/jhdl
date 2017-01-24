#include <cassert>
#include "../../ast/scanner.hpp"
#include "../scanner/defines.hpp"
#include "../scanner/scanner.hpp"
#include "entity.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Entity* Entity::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_ENTITY);
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      name = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_IS);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->skipOneOrMoreWhiteSpaces();
      if (scanner->optional(scanner::Scanner::VHDL_ENTITY)) {
	scanner->skipOneOrMoreWhiteSpaces();
      }
      i = scanner->expect<BasicIdentifier>();
      if (!name.equals(i->text)) {
        scanner->error("Identifier '" + i->text.toString() +
                       "' does not match entity name '" +
                       name.toString() + "'");
      }
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}


