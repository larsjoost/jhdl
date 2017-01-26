
#ifndef AST_VARIABLE_DECLARATION_H_
#define AST_VARIABLE_DECLARATION_H_

#include "text.hpp"

namespace ast {
    
  class VariableDeclaration {
    
  public:

    Text identifier;
    Text type;
    
  };

}

#endif
