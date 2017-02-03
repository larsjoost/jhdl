
#ifndef AST_EXPRESSION_TERM_H_
#define AST_EXPRESSION_TERM_H_

#include "number.hpp"
#include "string.hpp"
#include "basic_identifier.hpp"
#include "physical.hpp"

namespace ast {

  
  class ExpressionTerm {
    
  public:

    Physical* physical;
    Number* number;
    String* text;
    BasicIdentifier* identifier;

  };

}

#endif
