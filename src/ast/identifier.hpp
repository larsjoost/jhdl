
#ifndef AST_IDENTIFIER_H_
#define AST_IDENTIFIER_H_

#include <stdio.h>

#include "text.hpp"

namespace ast {

  class Identifier {
  public:
    Text text;
    void print(FILE* output = stdout);
    void debug(FILE* output = stdout);
    int equals(Identifier& t);
  };

}

#endif
