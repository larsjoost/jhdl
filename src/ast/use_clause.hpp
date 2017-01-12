#ifndef AST_USECLAUSE_HPP_
#define AST_USECLAUSE_HPP_

#include "list.hpp"
#include "text.hpp"

namespace ast {
    
  class UseClause {

  public:
    List<Text> list;
    
  };
}

#endif 
