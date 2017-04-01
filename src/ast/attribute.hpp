
#ifndef AST_ATTRIBUTE_H_
#define AST_ATTRIBUTE_H_

#include "simple_identifier.hpp"

namespace ast {
    
  class Attribute {
    
  public:

    SimpleIdentifier* identifier;
    SimpleIdentifier* typeMark;
    
  };

}

#endif
