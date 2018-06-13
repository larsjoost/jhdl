
#ifndef AST_TYPE_DECLARATION_H_
#define AST_TYPE_DECLARATION_H_

#include "simple_identifier.hpp"
#include "type_definition.hpp"
#include "interface_list.hpp"
#include "list.hpp"
#include "record_declaration.hpp"

namespace ast {

  class Declaration;
  class TypeDefinition;
  
  class TypeDeclaration {
    
  public:

    SimpleIdentifier* accessType = NULL;
    SimpleIdentifier* fileType = NULL;
    SimpleIdentifier* identifier;
    TypeDefinition* typeDefinition;
    RecordDeclaration* record;
    
  };

}

#endif
