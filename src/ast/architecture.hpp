
#ifndef AST_ARCHITECTURE_H_
#define AST_ARCHITECTURE_H_

#include "text.hpp"
#include "list.hpp"

namespace ast {
    
  class Architecture {
    
  public:
    
    Text entityName;
    Text architectureName;
    List<Text> port; 
    
  };

}

#endif
