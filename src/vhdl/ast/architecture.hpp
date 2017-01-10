
#ifndef VHDL_AST_ARCHITECTURE_H_
#define VHDL_AST_ARCHITECTURE_H_

#include "../../ast/text.hpp"

namespace vhdl {
  namespace ast {
    
    class Architecture {

    public:

      ::ast::Text entityName;
      ::ast::Text architectureName;

    };
  }
}

#endif
