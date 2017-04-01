
#ifndef AST_ARRAY_TYPE_H_
#define AST_ARRAY_TYPE_H_

#include "array_definition.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class ArrayType {
    
  public:

    ArrayDefinition* definition;
    SubtypeIndication* type;
    
  };

}

#endif
