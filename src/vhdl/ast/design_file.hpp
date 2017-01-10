#ifndef VHDL_AST_DESIGNFILE_HPP_
#define VHDL_AST_DESIGNFILE_HPP_

#include "../../ast/list.hpp"
#include "design_unit.hpp"

namespace vhdl {
  namespace ast {
  
    class DesignFile {
    public:
      ::ast::List<DesignUnit> designUnits;
    
    };
  }
}

#endif
