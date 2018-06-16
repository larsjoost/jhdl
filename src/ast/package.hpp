
#ifndef AST_PACKAGE_HPP_
#define AST_PACKAGE_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "declaration.hpp"
#include "interface_list.hpp"
#include "new_package_declaration.hpp"

namespace ast {
    
  class Package {
    
  public:

    SimpleIdentifier* name;
    InterfaceList* generics;
    bool body;
    List<Declaration> declarations;
    NewPackageDeclaration* new_package_declaration;
    
  };

}

#endif
