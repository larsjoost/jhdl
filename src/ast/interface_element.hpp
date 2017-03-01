
#ifndef AST_INTERFACEELEMENT_H_
#define AST_INTERFACEELEMENT_H_

#include "list.hpp"
#include "object_declaration.hpp"

namespace ast {
    
  class InterfaceElement {
    
  public:

    ObjectDeclaration* variable;
    ObjectDeclaration* signal;
    ObjectDeclaration* constant;
    
  };

}

#endif
