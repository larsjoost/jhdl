
#ifndef AST_PHYSICAL_H_
#define AST_PHYSICAL_H_

#include "number.hpp"
#include "simple_identifier.hpp"

namespace ast {

  class SimpleIdentifier;
  
  class Physical {
    
  public:

    Number* number;
    SimpleIdentifier* unit;

  };

}

#endif
