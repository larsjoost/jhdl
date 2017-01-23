
#ifndef AST_ASSOCIATION_LIST_H_
#define AST_ASSOCIATION_LIST_H_

#include "list.hpp"
#include "association_element.hpp"

namespace ast {
    
  class AssociationList {
    
  public:

    List<AssociationElement> associationElements;
    
  };

}

#endif
