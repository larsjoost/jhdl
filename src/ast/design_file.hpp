#ifndef AST_DESIGNFILE_HPP_
#define AST_DESIGNFILE_HPP_

#include <string>

#include "list.hpp"
#include "design_unit.hpp"

namespace ast {
  
  class DesignFile {
  public:
    std::string filename;
    List<DesignUnit> designUnits;
    
  };
}

#endif
