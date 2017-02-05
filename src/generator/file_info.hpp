#ifndef GENERATOR_FILE_INFO_HPP_
#define GENERATOR_FILE_INFO_HPP_

#include "../ast/design_file.hpp"
#include "../ast/basic_identifier.hpp"

namespace generator {
  
  class FileInfo { 
    std::string toString(ast::BasicIdentifier* i);
  public:
    FileInfo(ast::DesignFile& designFile);
  };
}

#endif


