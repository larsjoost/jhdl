#ifndef INFO_WRITER_HPP_
#define INFO_WRITER_HPP_

#include <string>
#include <fstream>
#include <regex>

#include "../debug/debug.hpp"

class InfoWriter {

  Debug<false> a_debug;

  std::string a_file_name;
  std::ofstream a_output_file;
  bool a_file_is_open = false;
  std::string a_expression = "VHDL_INCLUDES+=-I%p";
  std::string a_delimiter = "\n";
  
public:

  InfoWriter() : a_debug("InfoWriter") {}
  
  void Open(std::string file_name) {
    a_debug.functionStart("Open(file_name = " + file_name + ")");
    if (!a_file_is_open) {
      a_file_name = file_name;
      a_output_file.open(file_name);
      a_file_is_open = true;
    }
    a_debug.functionEnd("Open");
  }

  void Close() {
    a_debug.functionStart("Close");
    if (a_file_is_open) {
      a_output_file.close();
      a_file_is_open = false;
    }
    a_debug.functionEnd("Close");
  }
  
  void SetPackageName(const std::string& path, const std::string& name) {
    a_debug.functionStart("SetPackageName(path = " + path + ", name = " + name + ")");
    std::string x = a_expression;
    std::string o = std::regex_replace(x, std::regex("%p"), path);
    a_output_file << o << a_delimiter;
    a_debug.functionEnd("SetPackageName");
  }
  
  
};

#endif
