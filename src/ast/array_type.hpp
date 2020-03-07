
#ifndef AST_ARRAY_TYPE_H_
#define AST_ARRAY_TYPE_H_

#include "array_definition.hpp"
#include "subtype_indication.hpp"
#include "list.hpp"

namespace ast {
    
  class ArrayType {
    
  public:

    List<ArrayDefinition> array_definition;
    SubtypeIndication* subtype;
    
  };

}

#endif
