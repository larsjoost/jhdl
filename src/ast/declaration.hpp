
#ifndef AST_DECLARATION_H_
#define AST_DECLARATION_H_

#include "text.hpp"
#include "list.hpp"
#include "type_declaration.hpp"
#include "subtype_declaration.hpp"
#include "object_declaration.hpp"
#include "function_declaration.hpp"
#include "file_declaration.hpp"
#include "attribute.hpp"
#include "alias_declaration.hpp"

namespace ast {
    
  class FunctionDeclaration;
  class ProcedureDeclaration;
  class TypeDeclaration;
  
  class Declaration {
    
  public:

    TypeDeclaration* type;
    SubtypeDeclaration* subtype;
    ObjectDeclaration* variable;
    ObjectDeclaration* signal;
    ObjectDeclaration* constant;
    FunctionDeclaration* function;
    FileDeclaration* file;
    Attribute* attribute;
    AliasDeclaration* alias;
    
  };

}

#endif
