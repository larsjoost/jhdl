
#ifndef AST_CHOISE_H_
#define AST_CHOISE_H_

#include "text.hpp"
#include "list.hpp"
#include "expression.hpp"
#include "simple_identifier.hpp"
#include "range_direction_type.hpp"

namespace ast {
    
  class Choise {
    
  public:
    
    Text text;

    Expression* expression = NULL;
    RangeDirectionType* discrete_range = NULL;
    bool others = false;
    
  };

}

#endif
