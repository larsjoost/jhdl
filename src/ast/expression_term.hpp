
#ifndef AST_EXPRESSION_TERM_H_
#define AST_EXPRESSION_TERM_H_

#include <list>

#include "object_type.hpp"

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

    std::list<ObjectValueContainer> returnTypes;

    Text* text;

    List<Expression> parenthis;

    Physical* physical = NULL;
    Number* number = NULL;
    String* string = NULL;
    BasicIdentifier* identifier = NULL;
    Character* character = NULL;

  };

}

#endif
