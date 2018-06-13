
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_operator.hpp"
#include "unary_operator.hpp"
#include "expression.hpp"
#include "expression_term.hpp"

namespace vhdl {
  namespace parser {

    // expr = term <OP> term | unary | term
    // unary = <OP> term
    // term = expr | 'value' | ( expr )
    
    Expression* Expression::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = scanner->getCurrentTextPosition();
      if (unaryOperator = scanner->optional<UnaryOperator>()) {
        expression = scanner->expect<Expression, Expression>();
      } else if (term = scanner->optional<ExpressionTerm>()) {
        if (op = scanner->optional<ExpressionOperator>()) {
          expression = scanner->expect<Expression, Expression>();
        }
      }
      return this;
    }

  }
}

