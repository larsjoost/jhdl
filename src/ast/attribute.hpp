
#ifndef AST_ATTRIBUTE_H_
#define AST_ATTRIBUTE_H_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "string.hpp"
#include "object_type.hpp"
#include "string.hpp"

namespace ast {
    
  class Attribute {
    
  public:

    SimpleIdentifier* identifier;
    SimpleIdentifier* item;
    String* string;
    List<SimpleIdentifier>* arguments;
    ObjectType objectType;
    SimpleIdentifier* typeMark;
    String* expression;

    std::string toString(bool setCase = false) {
      return identifier->toString(setCase);
    }
    
  };

}

#endif
