
#ifndef AST_SIGNAL_DECLARATION_H_
#define AST_SIGNAL_DECLARATION_H_

#include "basic_identifier.hpp"
#include "declaration_initialization.hpp"

namespace ast {
    
  class SignalDeclaration {
    
  public:

    BasicIdentifier* identifier;
    BasicIdentifier* type;
    DeclarationInitialization* initialization;
    
  };

}

#endif
