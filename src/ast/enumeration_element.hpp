
#ifndef AST_ENUMERATION_ELEMENT_H_
#define AST_ENUMERATION_ELEMENT_H_

#include "simple_identifier.hpp"
#include "character.hpp"

namespace ast {
    
  class EnumerationElement {
    
  public:

    Character* character;
    SimpleIdentifier* identifier;
    
  };

}

#endif
