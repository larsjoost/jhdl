
#ifndef AST_TARGET_ARGUMENT_HPP_
#define AST_TARGET_ARGUMENT_HPP_

#include "expression.hpp"
#include "range_type.hpp"

namespace ast {
    
  class TargetArgument {
    
  public:

    RangeType* range = NULL;
    Expression* index = NULL;
    
  };

}

#endif
