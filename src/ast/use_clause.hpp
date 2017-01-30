#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "basic_identifier_list.hpp"

namespace ast {
    
  class UseClause {

  public:
    BasicIdentifierList* list;
    
  };
}

#endif 
