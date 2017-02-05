#include <iostream>
#include <list>
#include <cassert>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"

#include "file_info.hpp"

namespace generator {

  std::string FileInfo::toString(ast::BasicIdentifier* i) {
    assert (i != NULL);
    std::string s = i->text.toString(true);
    return s;
  }

  FileInfo::FileInfo(ast::DesignFile& designFile) {
    for (ast::DesignUnit& u : designFile.designUnits.list) {
      if (u.module.interface) {
        std::cout << toString(u.module.interface->name);
      }
      
    }
  }
  
}
