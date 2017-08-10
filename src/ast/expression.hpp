
#ifndef AST_EXPRESSION_H_
#define AST_EXPRESSION_H_

#include <list>

#include "object_type.hpp"

#include "number.hpp"
#include "string.hpp"
#include "expression_operator.hpp"
#include "unary_operator.hpp"
#include "expression_term.hpp"

namespace ast {

  class ExpressionTerm;
  
  class Expression {
    
  public:

    std::list<ObjectValueContainer> returnTypes;
    
    Text* text;

    ExpressionTerm* term;
    ExpressionOperator* op;
    UnaryOperator* unaryOperator;
    Expression* expression;
    List<Expression> parenthis;
    
  };

}

#endif
