
#ifndef AST_RANGE_DIRECTION_TYPE_HPP_
#define AST_RANGE_DIRECTION_TYPE_HPP_

#include "expression.hpp"
#include "range_direction.hpp"

namespace ast {
    
  class RangeDirectionType {
    
  public:

    Expression* left;
    RangeDirection* range_direction;
    Expression* right;
    
  };

}

#endif
