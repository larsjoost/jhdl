
#ifndef AST_FUNCTIONDECLARATION_H_
#define AST_FUNCTIONDECLARATION_H_

#include "simple_identifier.hpp"
#include "interface_list.hpp"
#include "function_body.hpp"
#include "object_type.hpp"
#include "string.hpp"

namespace ast {

  class FunctionBody;
  
  class FunctionDeclaration {
    
  public:

    ObjectType type;
    SimpleIdentifier* name;
    String* string = NULL;
    SimpleIdentifier* returnType;
    InterfaceList* interface;
    FunctionBody* body;

  };

}

#endif
