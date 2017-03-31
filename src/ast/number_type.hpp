
#ifndef AST_NUMBER_TYPE_H_
#define AST_NUMBER_TYPE_H_

#include "range_type.hpp"
#include "physical_type.hpp"

namespace ast {
    
  class NumberType {
    
  public:

    RangeType* range;
    PhysicalType* physical;
    
  };

}

#endif
