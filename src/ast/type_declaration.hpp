
#ifndef AST_TYPE_DECLARATION_H_
#define AST_TYPE_DECLARATION_H_

#include "basic_identifier.hpp"
#include "type_definition.hpp"

namespace ast {
    
  class TypeDeclaration {
    
  public:

    BasicIdentifier* identifier;
    TypeDefinition* typeDefinition;
    
  };

}

#endif
