#ifndef AST_BASICIDENTIFIERLIST_HPP_
#define AST_BASICIDENTIFIERLIST_HPP_

#include "basic_identifier.hpp"
#include "list.hpp"

namespace ast {
    
  class BasicIdentifierList {

  public:
    List<BasicIdentifier> textList;

  };
}

#endif 
