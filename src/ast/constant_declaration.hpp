
#ifndef AST_CONSTANT_DECLARATION_H_
#define AST_CONSTANT_DECLARATION_H_

#include "basic_identifier.hpp"

namespace ast {
    
  class ConstantDeclaration {
    
  public:

    BasicIdentifier* identifier;
    BasicIdentifier* type;
    
  };

}

#endif
