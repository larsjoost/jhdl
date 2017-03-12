
#ifndef AST_BASICINDETIFIER_H_
#define AST_BASICINDETIFIER_H_

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"

namespace ast {

  class Expression;
  
  class BasicIdentifier {
  public:
    Text text;
    Text* attribute;
    List<Expression>* arguments; 
    
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
