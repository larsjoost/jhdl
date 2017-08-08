  
#ifndef AST_OBJECT_DECLARATION_H_
#define AST_OBJECT_DECLARATION_H_

#include "simple_identifier.hpp"
#include "declaration_initialization.hpp"
#include "subtype_indication.hpp"
#include "object_type.hpp"

namespace ast {
    
  class ObjectDeclaration {
    
  public:

    enum class Direction {IN, OUT, INOUT, BUFFER, NONE};
    
    ObjectType objectType;
    SimpleIdentifier* identifier;
    Direction direction = Direction::NONE;
    SubtypeIndication* type;
    DeclarationInitialization* initialization;
    
  };

}

#endif
