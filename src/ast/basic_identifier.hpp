
#ifndef AST_BASICINDETIFIER_H_
#define AST_BASICINDETIFIER_H_

#include <list>

#include "object_type.hpp"

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "association_list.hpp"

namespace ast {

  class Expression;
  class AssociationList;
  
  class BasicIdentifier {
  public:

    std::list<ObjectValueContainer> returnTypes;
    
    Text text;
    bool range_attribute = false;
    Text* attribute = NULL; 
    AssociationList* arguments;
    
    bool equals(BasicIdentifier* b) {
      if (b) {
	return text.equals(b->text);
      }
      return false;
    }

    std::string toString(bool setCase = false) {
      return text.toString(setCase);
    }

  };
  
}

#endif
