
#ifndef AST_ATTRIBUTE_H_
#define AST_ATTRIBUTE_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "string.hpp"

namespace ast {
    
  class Attribute {
    
  public:

    SimpleIdentifier* identifier;
    SimpleIdentifier* item;
    List<SimpleIdentifier>* arguments;
    SimpleIdentifier* typeMark;
    String* expression;
    
  };

}

#endif