
#ifndef AST_ASSOCIATION_ELEMENT_FORMAL_PART_HPP_
#define AST_ASSOCIATION_ELEMENT_FORMAL_PART_HPP_

#include "simple_identifier.hpp"

namespace ast {

  class SimpleIdentifier;
  
  class AssociationElementFormalPart {
    
  public:
    
    SimpleIdentifier* name;

    std::string toString(bool setCase = false) {
      return name->toString(setCase);
    }

  };

}

#endif
