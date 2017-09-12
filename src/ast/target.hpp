
#ifndef AST_TARGET_HPP_
#define AST_TARGET_HPP_

#include "expression.hpp"
#include "range_type.hpp"
#include "simple_identifier.hpp"

namespace ast {
    
  class Target {
    
  public:

    SimpleIdentifier* identifier;
    RangeType* range = NULL;
    Expression* index = NULL;
    
  };

}

#endif
