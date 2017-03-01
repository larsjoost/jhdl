
#ifndef AST_OBJECT_DECLARATION_H_
#define AST_OBJECT_DECLARATION_H_

#include "basic_identifier.hpp"
#include "declaration_initialization.hpp"

namespace ast {
    
  class ObjectDeclaration {
    
  public:

    enum ObjectType {SIGNAL, VARIABLE, CONSTANT};

    ObjectType objectType;
    BasicIdentifier* identifier;
    BasicIdentifier* type;
    DeclarationInitialization* initialization;
    
  };

}

#endif
