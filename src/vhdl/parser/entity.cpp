
#include "../scanner/defines.hpp"
#include "../scanner/scanner.hpp"
#include "entity.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    void Entity::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      scanner->accept(scanner::VHDL_ENTITY);
      scanner->skipOneOrMoreWhiteSpaces();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      name = i->text;
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VHDL_IS);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(scanner::VHDL_END);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->optional(scanner::VHDL_ENTITY);
      scanner->skipOneOrMoreWhiteSpaces();
      scanner->expect(name);
      scanner->skipWhiteSpace();
      scanner->expect(";");
    }

  }
}


