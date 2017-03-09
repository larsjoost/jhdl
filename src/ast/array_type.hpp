
#ifndef AST_ARRAY_TYPE_H_
#define AST_ARRAY_TYPE_H_

#include "range_type.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class ArrayType {
    
  public:

    RangeType* range;
    SubtypeIndication* type;
    
  };

}

#endif
