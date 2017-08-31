
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_term.hpp"
#include "expression.hpp"
#include "basic_identifier.hpp"
#include "number.hpp"
#include "character.hpp"
#include "string.hpp"
#include "physical.hpp"
#include "element_association.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionTerm* ExpressionTerm::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = scanner->getCurrentTextPosition();
      if (scanner->optional("(")) {
        do {
          parenthis.add(scanner->expect<ElementAssociation>());
        } while (scanner->optional(","));
        scanner->expect(")");
      } else {
        ((physical = scanner->optional<Physical>()) ||
         (number = scanner->optional<Number>()) ||
         (character = scanner->optional<Character>()) ||
         (string = scanner->optional<String>()) || 
         (identifier = scanner->optional<BasicIdentifier>()));
      }
      return this;
    }

  }
}

