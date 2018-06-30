
#ifndef AST_BASICIDENTIFIER_HPP_
#define AST_BASICIDENTIFIER_HPP_

#include <cassert>
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
    
    SimpleIdentifier* identifier;
    BasicIdentifier* element;
    bool range_attribute = false;
    Text* attribute = NULL; 
    Expression* type_select = NULL;
    AssociationList* arguments = NULL;
    RangeType* range = NULL;
    
    bool equals(BasicIdentifier* b) {
      if (b) {
	return identifier->text.equals(b->identifier->text);
      }
      return false;
    }

    Text& getText() {
      return identifier->text;
    }
    
    std::string toString(bool setCase = false) {
      assert (identifier);
      return identifier->text.toString(setCase);
    }

  };
  
}

#endif
