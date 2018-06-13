
#ifndef AST_ELEMENTDECLARATIONLIST_H_
#define AST_ELEMENTDECLARATIONLIST_H_

#include "list.hpp"
#include "element_declaration.hpp"

namespace ast {
    
  class ElementDeclarationList {
    
  public:

    List<ElementDeclaration> element_declaration;

  };

}

#endif
