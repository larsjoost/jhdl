#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <string>

#include "../output/output.hpp"

class Debug {

  bool a_verbose;
  std::string a_name;
  int a_indent = 0;
  std::string a_function_name;
  const int INDENT_SIZE = 2;

  Output a_output = Output(std::cout);
  
  void print(std::string name, const std::string type, bool highlight, bool indent_increase);
  void print(std::string name, const std::string type, bool highlight);
  
public:
  Debug(std::string name, bool verbose);
  
  void functionStart(std::string name, bool highlight = false);
  void functionEnd(std::string name, bool highlight = false);

  void debug(std::string name, bool highlight = false);
  
};

#endif
