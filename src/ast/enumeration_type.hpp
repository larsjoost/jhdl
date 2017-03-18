
#ifndef AST_ENUMERATION_TYPE_H_
#define AST_ENUMERATION_TYPE_H_

#include "simple_identifier.hpp"
#include "list.hpp"

namespace ast {
    
  class EnumerationType {
    
  public:

    List<SimpleIdentifier>* enumerations;
    
  };

}

#endif
