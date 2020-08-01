#include <iostream>
#include <list>
#include <cassert>
#include <filesystem>
#include <ostream>
#include <fstream>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"

#include "file_info.hpp"

namespace generator {

  FileInfo::FileInfo(std::string& file_name) {
    m_file_name = file_name;
  }

  std::string FileInfo::replace_extension(std::string file_name, std::string file_extension) {
    std::filesystem::path p = file_name;
    p.replace_extension(file_extension);
    return p.string();
  }
    
  void FileInfo::write_dependencies(ast::DesignFile& designFile, std::string file_extension) {
    std::ofstream file_handle;
    std::string file_name = replace_extension(m_file_name, file_extension);
    file_handle.open(file_name);
    for (ast::DesignUnit& u : designFile.designUnits.list) {
      if (u.module.interface && u.module.interface->name) {
        file_handle << u.module.interface->name->toString(true);
      }
    }
    file_handle.close();
  }
  
  void FileInfo::write_makefile_dependencies(ast::DesignFile& designFile, std::string file_extension) {
    std::ofstream file_handle;
    std::string file_name = replace_extension(m_file_name, file_extension);
    file_handle.open(file_name);
    file_handle << m_file_name << " :";
    for (ast::DesignUnit& u : designFile.designUnits.list) {
      if (u.module.interface && u.module.interface->name) {
        file_handle << " " << u.module.interface->name->toString(true) << " \\" << std::endl;
      }
    }
    file_handle.close();
  }

}
