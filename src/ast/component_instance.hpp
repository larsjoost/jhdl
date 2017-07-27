
#ifndef AST_COMPONENTINSTANCE_HPP_
#define AST_COMPONENTINSTANCE_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "association_list.hpp"

namespace ast {
    
  class ComponentInstance {
    
  public:

    SimpleIdentifier* instanceName = NULL;
    SimpleIdentifier* componentName = NULL;
    SimpleIdentifier* libraryName = NULL;
    SimpleIdentifier* architectureName = NULL;
    AssociationList* generics = NULL;
    AssociationList* ports = NULL; 

  };

}

#endif
