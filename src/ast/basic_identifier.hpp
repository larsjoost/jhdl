
#ifndef AST_BASICINDETIFIER_H_
#define AST_BASICINDETIFIER_H_

#include <list>

#include "object_type.hpp"

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "association_list.hpp"
#include "range_type.hpp"

namespace ast {

  class Expression;
  class AssociationList;
  class RangeType;
  
  class BasicIdentifier {
  public:

    ReturnTypes returnTypes;
    
    Text text;
    bool range_attribute = false;
    Text* attribute = NULL; 
    Expression* type_select = NULL;
    AssociationList* arguments = NULL;
    RangeType* range = NULL;
    
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
