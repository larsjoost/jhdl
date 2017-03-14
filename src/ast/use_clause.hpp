#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "list.hpp"
#include "simple_identifier.hpp"

namespace ast {
    
  class UseClause {

  public:
    List<SimpleIdentifier>* list;
    
  };
}

#endif 
