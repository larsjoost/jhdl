#ifndef VHDL_AST_USECLAUSE_HPP_
#define VHDL_AST_USECLAUSE_HPP_

#include "../../ast/list.hpp"
#include "../../ast/text.hpp"

namespace vhdl {
  namespace ast {
    
    class UseClause {

    public:
      ::ast::List<::ast::Text> list;

    };
  }
}

#endif 
