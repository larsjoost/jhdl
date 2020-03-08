#ifndef INFO_WRITER_HPP_
#define INFO_WRITER_HPP_

#include <string>
#include <fstream>
#include <regex>

#include "../debug/debug.hpp"

class InfoWriter {

  Debug<false> m_debug;

  std::string m_file_name;
  std::ofstream m_output_file;
  bool m_file_is_open = false;
  
public:

  InfoWriter() : m_debug(this) {}
  
  void Open(std::string file_name) {
    m_debug.functionStart("Open(file_name = " + file_name + ")");
    if (!m_file_is_open) {
      m_file_name = file_name;
      m_output_file.open(file_name);
      m_file_is_open = true;
    }
    m_debug.functionEnd("Open");
  }

  void Close() {
    m_debug.functionStart("Close");
    if (m_file_is_open) {
      m_output_file.close();
      m_file_is_open = false;
    }
    m_debug.functionEnd("Close");
  }
  
  void SetPackageName(const std::string& path, const std::string& name) {
    m_debug.functionStart("SetPackageName(path = " + path + ", name = " + name + ")");
    m_output_file << path << std::endl;
    m_debug.functionEnd("SetPackageName");
  }
  
  
};

#endif
