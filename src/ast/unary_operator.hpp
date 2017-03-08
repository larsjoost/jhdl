
#ifndef AST_UNARY_OPERATOR_H_
#define AST_UNARY_OPERATOR_H_

namespace ast {
    
  class UnaryOperator {
    
  public:

    enum operators {
      NOT, MINUS};
    
    operators op;
    
  };

}

#endif
