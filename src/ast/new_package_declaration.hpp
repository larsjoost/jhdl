
#ifndef AST_NEW_PACKAGE_DECLARATION_HPP_
#define AST_NEW_PACKAGE_DECLARATION_HPP_

#include "simple_identifier.hpp"
#include "association_list.hpp"

namespace ast {
    
  class NewPackageDeclaration {
    
  public:

    SimpleIdentifier* library;
    SimpleIdentifier* package;
    AssociationList* generics = NULL;
    
  };

}

#endif
