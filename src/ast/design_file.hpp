#ifndef AST_DESIGNFILE_HPP_
#define AST_DESIGNFILE_HPP_

#include "list.hpp"
#include "design_unit.hpp"

namespace ast {
  
  class DesignFile {
  public:
    const char* filename;
    List<DesignUnit> designUnits;
    
  };
}

#endif
