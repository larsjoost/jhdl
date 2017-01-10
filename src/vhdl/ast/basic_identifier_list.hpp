#ifndef VHDL_AST_BASICIDENTIFIERLIST_HPP_
#define VHDL_AST_BASICIDENTIFIERLIST_HPP_

#include "../../ast/text.hpp"
#include "../../ast/list.hpp"

namespace vhdl {
  namespace ast {
    
    class BasicIdentifierList {

    public:
      ::ast::List<::ast::Text> textList;

    };
  }
}

#endif 
