#ifndef AST_DESIGNFILE_HPP_
#define AST_DESIGNFILE_HPP_

#include <string>

#include "list.hpp"
#include "design_unit.hpp"

namespace ast {
  
  class DesignFile {

  public:

    enum class LanguageType {VHDL, VERILOG};

  protected:
    
    LanguageType a_language_type;

  public:

    bool IsLanguage(LanguageType language_type) {return language_type == a_language_type;}
    
    std::string filename;
    List<DesignUnit> designUnits;
    
  };
}

#endif
