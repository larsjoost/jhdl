
#ifndef AST_SUBTYPE_INDICATION_H_
#define AST_SUBTYPE_INDICATION_H_

#include "simple_identifier.hpp"
#include "range_type.hpp"

namespace ast {
    
  class SubtypeIndication {
    
  public:

    SimpleIdentifier* name;
    RangeType* range;
    
  };

}

#endif
