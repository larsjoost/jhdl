
#ifndef AST_CHARACTER_H_
#define AST_CHARACTER_H_

#include <string>

#include "text.hpp"


namespace ast {
    
  class Character {
    
  public:
    
    Text value;

    std::string toString() {
      return value.toString();
    }
    
  };

}

#endif
