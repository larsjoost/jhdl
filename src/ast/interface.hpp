
#ifndef AST_INTERFACE_H_
#define AST_INTERFACE_H_

#include "basic_identifier.hpp"
#include "list.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Interface {
    
  public:

    BasicIdentifier* name;
    List<Declaration> generics;
    List<Declaration> ports;
    
  };

}

#endif
