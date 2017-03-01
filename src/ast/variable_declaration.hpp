
#ifndef AST_VARIABLE_DECLARATION_H_
#define AST_VARIABLE_DECLARATION_H_

#include "basic_identifier.hpp"
#include "declaration_initialization.hpp"

namespace ast {
    
  class VariableDeclaration {
    
  public:

    BasicIdentifier* identifier;
    BasicIdentifier* type;
    DeclarationInitialization* initialization;
    
  };

}

#endif
