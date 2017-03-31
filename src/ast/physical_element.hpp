
#ifndef AST_PHYSICALELEMENT_HPP_
#define AST_PHYSICALELEMENT_HPP_

#include "physical.hpp"
#include "simple_identifier.hpp"

namespace ast {

  class BasicIdentifier;
  
  class PhysicalElement {
    
  public:

    SimpleIdentifier* unit;
    Physical* physical;

  };

}

#endif
