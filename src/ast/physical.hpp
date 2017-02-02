
#ifndef AST_PHYSICAL_H_
#define AST_PHYSICAL_H_

#include "number.hpp"
#include "basic_identifier.hpp"

namespace ast {

  
  class Physical {
    
  public:

    Number* number;
    BasicIdentifier* unit;

  };

}

#endif
