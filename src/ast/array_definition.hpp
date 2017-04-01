
#ifndef AST_ARRAY_DEFINITION_H_
#define AST_ARRAY_DEFINITION_H_

#include "range_type.hpp"
#include "array_subtype_definition.hpp"

namespace ast {
    
  class ArrayDefinition {
    
  public:

    RangeType* range;
    ArraySubtypeDefinition* subtype;
    
  };

}

#endif
