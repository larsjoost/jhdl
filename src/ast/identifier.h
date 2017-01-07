
#ifndef AST_IDENTIFIER_H_
#define AST_IDENTIFIER_H_

#include <stdio.h>

#include "Text.h"

namespace ast {

  class Identifier {
  public:
    Text text;
    void print(FILE* output = stdout);
    int equals(Identifier& t);
  };

}

#endif
