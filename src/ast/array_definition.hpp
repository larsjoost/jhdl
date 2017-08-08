
#ifndef AST_ARRAY_DEFINITION_H_
#define AST_ARRAY_DEFINITION_H_

#include "range_type.hpp"
#include "array_subtype_definition.hpp"
#include "simple_identifier.hpp"

namespace ast {
    
  class ArrayDefinition {
    
  public:

    SimpleIdentifier* identifier = NULL;
    RangeType* range = NULL;
    ArraySubtypeDefinition* subtype = NULL;
    
  };

}

#endif
