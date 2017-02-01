
#ifndef AST_RANGE_TYPE_H_
#define AST_RANGE_TYPE_H_

#include "expression.hpp"
#include "range_direction.hpp"

namespace ast {
    
  class RangeType {
    
  public:

    Expression* left;
    RangeDirection* rangeDirection;
    Expression* right;
    
  };

}

#endif
