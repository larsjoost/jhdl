
#ifndef AST_OBJECT_DECLARATION_H_
#define AST_OBJECT_DECLARATION_H_

#include "basic_identifier.hpp"
#include "declaration_initialization.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class ObjectDeclaration {
    
  public:

    enum ObjectType {SIGNAL, VARIABLE, CONSTANT};

    enum Direction {IN, OUT, INOUT, BUFFER, NONE};
    
    ObjectType objectType;
    BasicIdentifier* identifier;
    Direction direction = NONE;
    BasicIdentifier* type;
    SubtypeIndication* subtype;
    DeclarationInitialization* initialization;
    
  };

}

#endif
