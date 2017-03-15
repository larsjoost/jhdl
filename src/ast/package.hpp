
#ifndef AST_PACKAGE_HPP_
#define AST_PACKAGE_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Package {
    
  public:

    SimpleIdentifier* name;
    List<Declaration> declarations;
    
  };

}

#endif
