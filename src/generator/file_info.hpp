#ifndef GENERATOR_FILE_INFO_HPP_
#define GENERATOR_FILE_INFO_HPP_

#include <ostream>

#include "../ast/design_file.hpp"
#include "../ast/basic_identifier.hpp"

namespace generator {
  
  class FileInfo { 
    std::string m_file_name;

    std::string replace_extension(std::string file_name, std::string file_extension);
  public:
    FileInfo(std::string& file_name);
    
    void write_dependencies(ast::DesignFile& designFile, std::string file_extension);
    void write_makefile_dependencies(ast::DesignFile& designFile, std::string file_extension);

  };
}

#endif


