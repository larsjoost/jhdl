
#ifndef AST_INTERFACE_H_
#define AST_INTERFACE_H_

#include "text.hpp"
#include "list.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Interface {
    
  public:

    Text name;
    List<Declaration> generics;
    List<Declaration> ports;
    
  };

}

#endif
