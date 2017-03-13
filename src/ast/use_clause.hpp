#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "list.hpp"
#include "basic_identifier.hpp"

namespace ast {
    
  class UseClause {

  public:
    List<BasicIdentifier>* list;
    
  };
}

#endif 
