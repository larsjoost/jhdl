
#ifndef AST_INTERFACE_H_
#define AST_INTERFACE_H_

#include "basic_identifier.hpp"
#include "interface_list.hpp"

namespace ast {
    
  class Interface {
    
  public:

    BasicIdentifier* name;
    InterfaceList* generics;
    InterfaceList* ports;
    
  };

}

#endif
