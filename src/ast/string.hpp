
#ifndef AST_STRING_H_
#define AST_STRING_H_

#include "text.hpp"

namespace ast {
    
  class String {
    
  public:
    
    Text text;

    std::string toString() {
      return text.toString();
    }
    
  };

}

#endif
