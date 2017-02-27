
#ifndef AST_INTERFACELIST_H_
#define AST_INTERFACELIST_H_

#include "list.hpp"
#include "interface_element.hpp"

namespace ast {
    
  class InterfaceList {
    
  public:

    List<InterfaceElement> interfaceElements;
    
  };

}

#endif
