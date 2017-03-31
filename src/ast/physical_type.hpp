
#ifndef AST_PHYSICALTYPE_HPP_
#define AST_PHYSICALTYPE_HPP_

#include "physical_element.hpp"
#include "list.hpp"

namespace ast {

  class BasicIdentifier;
  
  class PhysicalType {
    
  public:

    List<PhysicalElement> elements;

  };

}

#endif
