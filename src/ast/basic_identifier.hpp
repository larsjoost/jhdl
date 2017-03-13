
#ifndef AST_BASICINDETIFIER_H_
#define AST_BASICINDETIFIER_H_

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "association_list.hpp"

namespace ast {

  class Expression;
  class AssociationList;
  
  class BasicIdentifier {
  public:
    Text text;
    Text* attribute = NULL; 
    AssociationList* arguments;
    
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
