#ifndef _EXCEPTIONS_HPP
#define _EXCEPTIONS_HPP

#include <cstdio>
#include <string>
#include <unistd.h>

#include "../ast/text.hpp"

class Exceptions {

  const static int RESET = 0;
  const static int RED = 31;
  const static int GREEN = 32; 
  const static int YELLOW = 34; 
  const static int BLUE = 34; 
  
  static bool colorsSupported;

  static int numberOfErrors;
  static int numberOfWarnings;
  
  std::string colorCode(int color);
  void print(std::string severity, int color, std::string& message, ast::Text* text);

public:

  Exceptions() {
    colorsSupported = isatty(fileno(stdout));
  };
  
  void printError(std::string message, ast::Text* text = NULL);
  void printWarning(std::string message, ast::Text* text = NULL);

};

#endif
