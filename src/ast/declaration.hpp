
#ifndef AST_DECLARATION_H_
#define AST_DECLARATION_H_

#include "text.hpp"
#include "list.hpp"
#include "type_declaration.hpp"
#include "variable_declaration.hpp"

namespace ast {
    
  class Declaration {
    
  public:

    TypeDeclaration* type;
    VariableDeclaration* variable;
    
  };

}

#endif