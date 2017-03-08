
#ifndef AST_DECLARATION_H_
#define AST_DECLARATION_H_

#include "text.hpp"
#include "list.hpp"
#include "type_declaration.hpp"
#include "subtype_declaration.hpp"
#include "object_declaration.hpp"
#include "function_declaration.hpp"

namespace ast {
    
  class FunctionDeclaration;

  class Declaration {
    
  public:

    TypeDeclaration* type;
    SubtypeDeclaration* subtype;
    ObjectDeclaration* variable;
    ObjectDeclaration* signal;
    ObjectDeclaration* constant;
    FunctionDeclaration* function;
    
  };

}

#endif
