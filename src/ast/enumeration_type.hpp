
#ifndef AST_ENUMERATION_TYPE_H_
#define AST_ENUMERATION_TYPE_H_

#include "enumeration_element.hpp"
#include "list.hpp"

namespace ast {
    
  class EnumerationType {
    
  public:

    List<EnumerationElement>* enumerations;
    
  };

}

#endif
