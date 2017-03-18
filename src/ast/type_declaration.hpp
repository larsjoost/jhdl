
#ifndef AST_TYPE_DECLARATION_H_
#define AST_TYPE_DECLARATION_H_

#include "simple_identifier.hpp"
#include "type_definition.hpp"

namespace ast {
    
  class TypeDeclaration {
    
  public:

    SimpleIdentifier* identifier;
    TypeDefinition* typeDefinition;
    
  };

}

#endif
