
#ifndef AST_NUMBER_H_
#define AST_NUMBER_H_

#include <string>

#include "text.hpp"

namespace ast {
    
  class Number {
    
  public:
    
    Text value;

    std::string toString() {
      return value.toString();
    }
    
  };

}

#endif
