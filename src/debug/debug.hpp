#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <string>
#include <iostream>

#include "../output/output.hpp"


class Debug {

protected:
  
  const static int DEFAULT_HIGHLIGHT_COLOR = Output::GREEN;

public:
  
  virtual void functionStart(std::string name, bool highlight = false) = 0;
  virtual void functionEnd(std::string name, bool highlight = false) = 0;

  virtual void debug(std::string name, bool highlight = false, int highlight_color = DEFAULT_HIGHLIGHT_COLOR) = 0;
  
};

class EnableDebug : public Debug {

  std::string a_name;
  int a_indent = 0;
  std::string a_function_name;
  const int INDENT_SIZE = 2;

  
  Output a_output = Output(std::cout);
  
  void print(std::string name, const std::string type, bool highlight, bool indent_increase);
  void print(std::string name, const std::string type, bool highlight, int highlight_color = DEFAULT_HIGHLIGHT_COLOR);
  
public:

  EnableDebug(std::string name);
  
  void functionStart(std::string name, bool highlight = false);
  void functionEnd(std::string name, bool highlight = false);

  void debug(std::string name, bool highlight = false, int highlight_color = DEFAULT_HIGHLIGHT_COLOR);
  
};

class DisableDebug : public Debug {

public:
  
  DisableDebug(std::string name) {};
  
  void functionStart(std::string name, bool highlight = false) {};
  void functionEnd(std::string name, bool highlight = false) {};

  void debug(std::string name, bool highlight = false, int highlight_color = DEFAULT_HIGHLIGHT_COLOR) {};

};

#endif
