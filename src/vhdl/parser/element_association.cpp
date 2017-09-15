
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "element_association.hpp"
#include "choises.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ElementAssociation* ElementAssociation::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      choises = scanner->optional<Choises>();
      if (scanner->optional("=>")) {
        expression = scanner->expect<Expression>();
      }
      return this;
    }

  }
}

