
#ifndef AST_SUBTYPE_INDICATION_H_
#define AST_SUBTYPE_INDICATION_H_

#include "basic_identifier.hpp"
#include "range_type.hpp"

namespace ast {
    
  class SubtypeIndication {
    
  public:

    BasicIdentifier* name;
    RangeType* range;
    
  };

}

#endif
