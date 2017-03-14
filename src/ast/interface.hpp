
#ifndef AST_INTERFACE_H_
#define AST_INTERFACE_H_

#include "simple_identifier.hpp"
#include "interface_list.hpp"

namespace ast {
    
  class Interface {
    
  public:

    SimpleIdentifier* name;
    InterfaceList* generics;
    InterfaceList* ports;
    
  };

}

#endif
