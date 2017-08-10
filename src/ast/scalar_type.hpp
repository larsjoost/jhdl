
#ifndef AST_SCALAR_TYPE_H_
#define AST_SCALAR_TYPE_H_

#include "number.hpp"
#include "string.hpp"
#include "simple_identifier.hpp"
#include "physical.hpp"
#include "character.hpp"


namespace ast {

  class ScalarType {
    
  public:

    Text* text;

    Physical* physical;
    Number* number;
    String* string;
    SimpleIdentifier* identifier;
    Character* character;
    
  };

}

#endif
