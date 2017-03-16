
#ifndef AST_FUNCTIONDECLARATION_H_
#define AST_FUNCTIONDECLARATION_H_

#include "simple_identifier.hpp"
#include "interface_list.hpp"
#include "function_body.hpp"

namespace ast {

  class FunctionBody;
  
  class FunctionDeclaration {
    
  public:
    
    SimpleIdentifier* name;
    SimpleIdentifier* returnType;
    InterfaceList* interface;
    FunctionBody* body;
    
  };

}

#endif
