
#ifndef AST_PORT_H_
#define AST_PORT_H_

#include "text.hpp"
#include "list.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Port {
    
  public:

    List<Declaration> declarations;
    
  };

}

#endif
