  
#ifndef AST_ELEMENT_DECLARATION_H_
#define AST_ELEMENT_DECLARATION_H_

#include "simple_identifier.hpp"
#include "subtype_indication.hpp"
#include "list.hpp"

namespace ast {
    
  class ElementDeclaration {
    
  public:

    List<SimpleIdentifier> identifiers;
    SubtypeIndication* type;
    
  };

}

#endif
