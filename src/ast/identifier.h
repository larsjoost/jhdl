
#ifndef AST_IDENTIFIER_H_
#define AST_IDENTIFIER_H_

#include "Text.h"

namespace ast {

  class Identifier {
  public:
    Text text;
    int equals(Identifier& t);
  };

}

#endif
