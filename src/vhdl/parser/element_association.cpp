
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "element_association.hpp"
#include "choises.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ElementAssociation* ElementAssociation::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = scanner->getCurrentTextPosition();
      do {
        choises = scanner->optional<Choises>();
        expression = scanner->expect<Expression>();
      } while (scanner->optional(","));
      return this;
    }

  }
}

