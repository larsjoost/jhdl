
#ifndef AST_TWO_TERM_EXPRESSION_H_
#define AST_TWO_TERM_EXPRESSION_H_

#include "expression.hpp"
#include "expression_operator.hpp"

namespace ast {

  class Expression;
  
  class TwoTermExpression {
    
  public:

    Expression* term1;
    ExpressionOperator* op;
    Expression* term2;
    
  };

}

#endif
