
#ifndef AST_SUBTYPE_DECLARATION_H_
#define AST_SUBTYPE_DECLARATION_H_

#include "simple_identifier.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class SubtypeDeclaration {
    
  public:

    SimpleIdentifier* identifier;
    SimpleIdentifier* resolution_function = NULL;
    SubtypeIndication* type;
    
  };

}

#endif
