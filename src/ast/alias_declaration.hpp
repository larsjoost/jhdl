
#ifndef AST_ALIAS_DECLARATION_H_
#define AST_ALIAS_DECLARATION_H_

#include "simple_identifier.hpp"
#include "subtype_indication.hpp"

namespace ast {
    
  class AliasDeclaration {
    
  public:

    SimpleIdentifier* designator;
    SimpleIdentifier* name;
    SubtypeIndication* type = NULL;
    
  };

}

#endif
