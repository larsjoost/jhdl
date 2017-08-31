
#ifndef AST_AGGREGATE_H_
#define AST_AGGREGATE_H_

#include "text.hpp"
#include "list.hpp"

namespace ast {
    
  class Aggregate {
    
  public:
    
    Text text;

    List<ElementAssociation> element_associations;
    
  };

}

#endif
