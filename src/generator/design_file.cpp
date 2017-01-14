#include <iostream>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"
#include "../ast/architecture.hpp"
#include "../ast/entity.hpp"
#include "../ast/context_clause.hpp"

#include "design_file.hpp"

namespace generator {

  DesignFile::DesignFile(::ast::DesignFile designFile) {
    std::cout << "// Filename : " << std::string(designFile.filename) << std::endl;
    for (ast::DesignUnit d : designFile.designUnits.list) {
      if (d.architecture) {
        std::string name = d.architecture->entityName.toString();
        std::cout << "#include \"systemc.h\"" << std::endl << std::endl;
        std::cout << "SC_MODULE(" << name << ")" << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "};" << std::endl;
      }
      
    }
  }

}
