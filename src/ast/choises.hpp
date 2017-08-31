
#ifndef AST_CHOISES_H_
#define AST_CHOISES_H_

#include "text.hpp"
#include "list.hpp"
#include "choise.hpp"

namespace ast {
    
  class Choises {
    
  public:
    
    Text text;

    List<Choise> choises;
    
  };

}

#endif
