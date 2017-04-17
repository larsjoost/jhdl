
#ifndef AST_NUMBER_H_
#define AST_NUMBER_H_

#include <string>

#include "text.hpp"
#include "object_type.hpp"

namespace ast {
    
  class Number {
    
  public:

    ObjectValue type;
    
    Text value;

    std::string toString() {
      return value.toString();
    }
    
  };

}

#endif
