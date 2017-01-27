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
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      name = i->text;
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_IS);
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->skipWhiteSpace();
      if (scanner->optional(scanner::Scanner::VHDL_ENTITY)) {
	scanner->skipWhiteSpace();
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


