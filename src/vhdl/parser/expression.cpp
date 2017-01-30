
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "expression_operator.hpp"
#include "expression.hpp"
#include "basic_identifier.hpp"
#include "number.hpp"
#include "string.hpp"

namespace vhdl {
  namespace parser {
  
    Expression* Expression::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if ((term.number = scanner->optional<Number>()) ||
          (term.text = scanner->optional<String>()) ||
          (term.identifier = scanner->optional<BasicIdentifier>())) {
        scanner->skipWhiteSpace();
        if (op = scanner->optional<ExpressionOperator>()) {
          scanner->skipWhiteSpace();
          expression = scanner->expect<Expression>();
        }
      }
      return this;
    }

  }
}

