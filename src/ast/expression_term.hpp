
#ifndef AST_EXPRESSION_TERM_H_
#define AST_EXPRESSION_TERM_H_

#include <list>

#include "object_type.hpp"

#include "number.hpp"
#include "string.hpp"
#include "basic_identifier.hpp"
#include "physical.hpp"
#include "character.hpp"
#include "element_association.hpp"

namespace ast {

  class BasicIdentifier;
  class Physical;
  class ElementAssociation;
  
  class ExpressionTerm {
    
  public:

    ReturnTypes returnTypes;

    Text* text;

    List<ElementAssociation> parenthis;

    Physical* physical = NULL;
    Number* number = NULL;
    String* string = NULL;
    BasicIdentifier* identifier = NULL;
    Character* character = NULL;

  };

}

#endif
