
#ifndef AST_EXPRESSION_TERM_H_
#define AST_EXPRESSION_TERM_H_

#include "number.hpp"
#include "string.hpp"
#include "basic_identifier.hpp"

namespace ast {

  
  class ExpressionTerm {
    
  public:

    Number* number;
    String* text;
    BasicIdentifier* identifier;

  };

}

#endif
