
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "two_term_expression.hpp"
#include "expression.hpp"
#include "expression_operator.hpp"

namespace vhdl {
  namespace parser {
    
    TwoTermExpression* TwoTermExpression::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      term1 = scanner->accept<Expression>(); 
      op = scanner->accept<ExpressionOperator>();
      term2 = scanner->expect<Expression>();
      return this;
    }

  }
}

