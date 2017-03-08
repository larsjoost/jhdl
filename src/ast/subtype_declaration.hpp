
#ifndef AST_SUBTYPE_DECLARATION_H_
#define AST_SUBTYPE_DECLARATION_H_

#include "basic_identifier.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class SubtypeDeclaration {
    
  public:

    BasicIdentifier* identifier;
    SubtypeIndication* type;
    
  };

}

#endif
