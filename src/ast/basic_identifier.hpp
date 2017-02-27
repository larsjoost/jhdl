
#ifndef AST_BASICINDETIFIER_H_
#define AST_BASICINDETIFIER_H_

#include "text.hpp"
#include "basic_identifier.hpp"

namespace ast {

  class BasicIdentifierList;
  
  class BasicIdentifier {
  public:
    Text text;
    Text* attribute;
    BasicIdentifierList* arguments; 
    
    bool equals(BasicIdentifier* b) {
      if (b) {
	return text.equals(b->text);
      }
      return false;
    }

    std::string toString() {
      return text.toString();
    }

  };
  
}

#endif
