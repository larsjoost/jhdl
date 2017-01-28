
#ifndef AST_TYPE_DECLARATION_H_
#define AST_TYPE_DECLARATION_H_

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "type_definition.hpp"

namespace ast {
    
  class TypeDeclaration {
    
  public:

    Text identifier;
    TypeDefinition* typeDefinition;
    
  };

}

#endif
