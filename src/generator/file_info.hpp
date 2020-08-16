#ifndef GENERATOR_FILE_INFO_HPP_
#define GENERATOR_FILE_INFO_HPP_

#include <ostream>

#include "../debug/debug.hpp"

#include "../ast/design_file.hpp"
#include "../ast/basic_identifier.hpp"

namespace generator {
  
  class FileInfo { 

    Debug<false> m_debug;

    std::string m_file_name;
    std::string m_output_expression;

    void write(std::string& name, std::string& library, std::string& output_expression, std::string type);

  public:
    FileInfo(std::string& file_name, std::string& output_expression);

    static void help(int indent);
    
    void write_package(std::string& name, std::string& library);
    void write_package_body(std::string& name, std::string& library);
    void write_entity(std::string& name, std::string& library);
    void write_architecture(std::string& name, std::string& library);
    void write_package_dependency(std::string& name, std::string& library);
    void write_entity_dependency(std::string& name, std::string& library);
  };
}

#endif


