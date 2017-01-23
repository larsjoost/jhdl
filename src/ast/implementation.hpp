
#ifndef AST_IMPLEMENTATION_H_
#define AST_IMPLEMENTATION_H_

#include "text.hpp"
#include "list.hpp"
#include "declaration.hpp"
#include "method.hpp"

namespace ast {
    
  class Implementation {
    
  public:

    Text name;
    List<Declaration> declarations;
    List<Method> methods;
    
  };

}

#endif
