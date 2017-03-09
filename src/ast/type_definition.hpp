
#ifndef AST_TYPE_DEFINITION_H_
#define AST_TYPE_DEFINITION_H_

#include "number_type.hpp"
#include "enumeration_type.hpp"
#include "array_type.hpp"

namespace ast {
    
  class TypeDefinition {
    
  public:

    NumberType* numberType;
    EnumerationType* enumerationType;
    ArrayType* arrayType;
    
  };

}

#endif
