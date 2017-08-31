
#ifndef AST_CHOISE_H_
#define AST_CHOISE_H_

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "simple_identifier.hpp"
#include "range_direction.hpp"

namespace ast {
    
  class Choise {
    
  public:
    
    Text text;

    Expression* expression = NULL;
    RangeDirection* discrete_range = NULL;
    SimpleIdentifier* identifier = NULL;
    bool Others = false;
    
  };

}

#endif
