
#ifndef AST_EXPRESSION_OPERATOR_H_
#define AST_EXPRESSION_OPERATOR_H_

#include <string>

namespace ast {
    
  class ExpressionOperator {
    
  public:

    enum operators {
      CONCAT,
      ADD,
      EQUAL};
    
    operators op;
    
  };

}

#endif
