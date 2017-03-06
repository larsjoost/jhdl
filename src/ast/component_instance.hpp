
#ifndef AST_COMPONENTINSTANCE_HPP_
#define AST_COMPONENTINSTANCE_HPP_

#include "basic_identifier.hpp"
#include "basic_identifier_list.hpp"
#include "association_list.hpp"

namespace ast {
    
  class ComponentInstance {
    
  public:

    BasicIdentifier* instanceName = NULL;
    BasicIdentifierList* componentName = NULL;
    BasicIdentifier* architectureName = NULL;
    AssociationList* generics = NULL;
    AssociationList* ports = NULL; 

  };

}

#endif
