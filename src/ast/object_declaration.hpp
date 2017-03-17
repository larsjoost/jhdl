
#ifndef AST_OBJECT_DECLARATION_H_
#define AST_OBJECT_DECLARATION_H_

#include "simple_identifier.hpp"
#include "declaration_initialization.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class ObjectDeclaration {
    
  public:

    enum ObjectType {SIGNAL, VARIABLE, CONSTANT};

    enum Direction {IN, OUT, INOUT, BUFFER, NONE};
    
    ObjectType objectType;
    SimpleIdentifier* identifier;
    Direction direction = NONE;
    SubtypeIndication* type;
    DeclarationInitialization* initialization;
    
  };

}

#endif
