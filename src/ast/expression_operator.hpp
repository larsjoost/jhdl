
#ifndef AST_EXPRESSION_OPERATOR_H_
#define AST_EXPRESSION_OPERATOR_H_

#include <string>

namespace ast {
    
  class ExpressionOperator {
    
  public:

    enum operators {
      CONCAT,
      ADD,
      SUB,
      EQUAL,
      NOT_EQUAL,
      LARGER_OR_EQUAL,
      SMALLER_OR_EQUAL};
    
    operators op;
    
  };

}

#endif
