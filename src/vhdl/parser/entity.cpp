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
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      name = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_IS);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->skipOneOrMoreWhiteSpaces();
      if (scanner->optional(scanner::Scanner::VHDL_ENTITY)) {
	scanner->skipOneOrMoreWhiteSpaces();
      }
      scanner->expect(::ast::TOKEN_IDENTIFIER);
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}


