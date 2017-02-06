
#ifndef AST_SIGNAL_DECLARATION_H_
#define AST_SIGNAL_DECLARATION_H_

#include "basic_identifier.hpp"

namespace ast {
    
  class SignalDeclaration {
    
  public:

    BasicIdentifier* identifier;
    BasicIdentifier* type;
    
  };

}

#endif
