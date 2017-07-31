
#ifndef AST_TYPE_DECLARATION_H_
#define AST_TYPE_DECLARATION_H_

#include "simple_identifier.hpp"
#include "type_definition.hpp"

namespace ast {
    
  class TypeDeclaration {
    
  public:

    SimpleIdentifier* accessType = NULL;
    SimpleIdentifier* fileType = NULL;
    SimpleIdentifier* identifier;
    TypeDefinition* typeDefinition;
    
  };

}

#endif
