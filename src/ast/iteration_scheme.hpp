
#ifndef AST_ITERATION_SCHEME_H_
#define AST_ITERATION_SCHEME_H_

#include "range_type.hpp"

namespace ast {
    
  class IterationScheme {
    
  public:

    RangeType* range = NULL;
    SimpleIdentifier* identifier = NULL;
    bool range_attribute;
    bool reverse_range_attribute;
    
  };

}

#endif
