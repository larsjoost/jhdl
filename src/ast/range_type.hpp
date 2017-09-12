
#ifndef AST_RANGE_TYPE_H_
#define AST_RANGE_TYPE_H_

#include "expression.hpp"
#include "range_direction_type.hpp"
#include "range_attribute_type.hpp"

namespace ast {

  class RangeAttributeType;
  class RangeDirectionType;
  
  class RangeType {
    
  public:

    RangeAttributeType* range_attribute_type = NULL;
    RangeDirectionType* range_direction_type = NULL;
    
  };

}

#endif
