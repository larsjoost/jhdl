#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "character.hpp"
#include "simple_identifier.hpp"
#include "enumeration_element.hpp"

namespace vhdl {
  namespace parser {
  
    EnumerationElement* EnumerationElement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ((character = scanner->optional<Character>()) ||
       (identifier = scanner->optional<SimpleIdentifier>()));
      return this;
    }

  }
}
