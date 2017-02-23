
#ifndef AST_EXPRESSION_H_
#define AST_EXPRESSION_H_

#include "number.hpp"
#include "string.hpp"
#include "expression_operator.hpp"
#include "expression_term.hpp"

namespace ast {

  
  class Expression {
    
  public:

    ExpressionTerm term;
    ExpressionOperator* op;
    Expression* expression;
    Expression* parenthis;
    
  };

}

#endif
