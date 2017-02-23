
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "expression_operator.hpp"
#include "expression.hpp"
#include "basic_identifier.hpp"
#include "number.hpp"
#include "character.hpp"
#include "string.hpp"
#include "physical.hpp"

namespace vhdl {
  namespace parser {
  
    Expression* Expression::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->optional("(")) {
        parenthis = scanner->expect<Expression>();
        scanner->expect(")");
      } else {
        if ((term.physical = scanner->optional<Physical>()) ||
            (term.number = scanner->optional<Number>()) ||
            (term.character = scanner->optional<Character>()) ||
            (term.text = scanner->optional<String>()) ||
            (term.identifier = scanner->optional<BasicIdentifier>())) {
          if (op = scanner->optional<ExpressionOperator>()) {
            expression = scanner->expect<Expression>();
          }
        }
      }
      return this;
    }

  }
}

