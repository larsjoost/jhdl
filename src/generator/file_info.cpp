#include <iostream>
#include <list>
#include <cassert>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"

#include "file_info.hpp"

namespace generator {

  FileInfo::FileInfo(ast::DesignFile& designFile) {
    for (ast::DesignUnit& u : designFile.designUnits.list) {
      if (u.module.interface && u.module.interface->name) {
        std::cout << u.module.interface->name->toString(true);
      }
      
    }
  }
  
}
