
#ifndef AST_IDENTIFIER_H_
#define AST_IDENTIFIER_H_

#include <iostream>

#include "text.hpp"

namespace ast {

  class Identifier {
  public:
    Text text;
    void print(std::ostream& output = std::cout);
    void debug(std::ostream& output = std::cout);
    int equals(Identifier& t);
  };

}

#endif
