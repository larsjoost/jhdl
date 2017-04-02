
#ifndef AST_CHARACTER_H_
#define AST_CHARACTER_H_

#include <string>

#include "text.hpp"


namespace ast {
    
  class Character {
    
  public:
    
    Text value;

    std::string toString() {
      std::string s = value.toString();
      if (s.at(1) == '\'') {
        s = "'\\''";
      } else if (s.at(1) == '\\') {
        s = "'\\\\'";
      }
      return s;
    }
    
  };

}

#endif
