
#ifndef AST_ARRAY_SUBTYPE_DEFINITION_H_
#define AST_ARRAY_SUBTYPE_DEFINITION_H_

#include "simple_identifier.hpp"
#include "range_type.hpp"

namespace ast {
    
  class ArraySubtypeDefinition {
    
  public:

    SimpleIdentifier* identifier;
    RangeType* range = NULL;
    
  };

}

#endif
