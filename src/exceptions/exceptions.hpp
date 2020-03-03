#ifndef _EXCEPTIONS_HPP
#define _EXCEPTIONS_HPP

#include <cstdio>
#include <string>
#include <unistd.h>
#include <cstring>

#include "../ast/text.hpp"
#include "../output/output.hpp"

class Exceptions {

  bool verbose = false;

  Output m_output = Output(std::cerr);
  
  static int m_number_of_errors;
  static int m_number_of_warnings;

  const static bool a_display_warnings = true;
  
  void print(std::string severity, Output::Color color, std::string& message, ast::Text* text);

public:
  
  void printInternal(std::string message, ast::Text* text = NULL);
  void printNote(std::string message, ast::Text* text = NULL);
  void printError(std::string message, ast::Text* text = NULL, const char* file_name = NULL, int line_number = -1);
  void printError(std::string message, const char* file_name, int line_number);
  void printWarning(std::string message, ast::Text* text = NULL);
  bool errorsExists() {return m_number_of_errors > 0;};
  
};

#endif
