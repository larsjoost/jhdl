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

  Output a_output = Output(std::cerr);
  
  static int numberOfErrors;
  static int numberOfWarnings;

  const static bool a_display_warnings = false;
  
  void print(std::string severity, Output::Color color, std::string& message, ast::Text* text);

public:
  
  void printInternal(std::string message, ast::Text* text = NULL);
  void printNote(std::string message, ast::Text* text = NULL);
  void printError(std::string message, ast::Text* text = NULL);
  void printWarning(std::string message, ast::Text* text = NULL);

};

#endif
