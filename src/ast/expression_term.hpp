
#ifndef AST_EXPRESSION_TERM_H_
#define AST_EXPRESSION_TERM_H_

#include <list>

#include "number.hpp"
#include "string.hpp"
#include "basic_identifier.hpp"
#include "physical.hpp"
#include "character.hpp"

namespace ast {

  class BasicIdentifier;
  class Physical;

  class ExpressionTerm {
    
  public:

    Physical* physical;
    Number* number;
    String* text;
    BasicIdentifier* identifier;
    Character* character;
    
  };

}

#endif
