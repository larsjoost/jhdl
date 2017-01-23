#include <iostream>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"
#include "../ast/interface.hpp"
#include "../ast/context_clause.hpp"

#include "design_file.hpp"

namespace generator {

  DesignFile::DesignFile(::ast::DesignFile designFile) {
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    for (ast::DesignUnit d : designFile.designUnits.list) {
      if (d.module.interface) {
        std::string name = d.module.interface->name.toString();
        std::cout << "#include \"systemc.h\"" << std::endl << std::endl;
        std::cout << "SC_MODULE(" << name << ")" << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "};" << std::endl;
      }
      
    }
  }

}
