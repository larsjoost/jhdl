#include <iostream>
#include <list>
#include <cassert>
#include <filesystem>
#include <ostream>
#include <fstream>
#include <regex>

#include "../ast/design_file.hpp"
#include "../ast/design_unit.hpp"

#include "file_info.hpp"

namespace generator {

  FileInfo::FileInfo(std::string& file_name, std::string& output_expression) : m_debug(this) {
    m_file_name = file_name;
    m_output_expression = output_expression;
  }

  void FileInfo::help(int indent) {
    std::string s = std::string(indent, ' ');
    std::cout << s << "%t: Type" << std::endl;
    std::cout << s << "%f: File name" << std::endl;
    std::cout << s << "%u: Unit name (package and entity)" << std::endl;
    std::cout << s << "%l: Library name" << std::endl;
    std::cout << s << "%p: Package name" << std::endl;
    std::cout << s << "%b: Package body name" << std::endl;
    std::cout << s << "%e: Entity name" << std::endl;
    std::cout << s << "%a: Architecture name" << std::endl;
    std::cout << s << "%d: Dependent package name" << std::endl;
    std::cout << s << "%x: Dependent entity name" << std::endl;
  }
  
  void FileInfo::write(std::string& name, std::string& library, std::string& output_expression, std::string type) {
    if (!output_expression.empty()) {
      std::string x = output_expression;
      x = std::regex_replace(x, std::regex("%t"), type);
      x = std::regex_replace(x, std::regex("%f"), m_file_name);
      x = std::regex_replace(x, std::regex("%u"), name);
      x = std::regex_replace(x, std::regex("%l"), library);
      bool all_fields_replaced = (x.find("%") == std::string::npos);
      if (all_fields_replaced) {
	std::cout << x << std::endl;
      } else {
	m_debug.debug("Did not replace all fields in " + x);
      }
    }
  }

  void FileInfo::write_package(std::string& name, std::string& library) {
    if (!m_output_expression.empty()) {
      std::string x = std::regex_replace(m_output_expression, std::regex("%p"), name);
      write(name, library, x, "package");
    }
  }
  
  void FileInfo::write_package_body(std::string& name, std::string& library) {
    if (!m_output_expression.empty()) {
      std::string x = std::regex_replace(m_output_expression, std::regex("%b"), name);
      write(name, library, x, "package_body");
    }
  }

  void FileInfo::write_entity(std::string& name, std::string& library) {
    if (!m_output_expression.empty()) {
      std::string x = std::regex_replace(m_output_expression, std::regex("%e"), name);
      write(name, library, x, "entity");
    }
  }

  void FileInfo::write_architecture(std::string& name, std::string& library) {
    if (!m_output_expression.empty()) {
      std::string x = std::regex_replace(m_output_expression, std::regex("%a"), name);
      write(name, library, x, "architecture");
    }
  }

  void FileInfo::write_package_dependency(std::string& name, std::string& library) {
    if (!m_output_expression.empty()) {
      std::string x = std::regex_replace(m_output_expression, std::regex("%d"), name);
      write(name, library, x, "package_dependency");
    }
  }
  
  void FileInfo::write_entity_dependency(std::string& name, std::string& library) {
    if (!m_output_expression.empty()) {
      std::string x = std::regex_replace(m_output_expression, std::regex("%x"), name);
      write(name, library, x, "entity_dependency");
    }
  }

}
