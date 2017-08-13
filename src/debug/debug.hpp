#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <string>
#include <iostream>
#include <cassert>

#include "../output/output.hpp"

template<bool enable>
class Debug {

protected:
  
  const static Output::Color DEFAULT_HIGHLIGHT_COLOR = Output::Color::GREEN;

  std::string a_name;
  int a_indent = 0;
  const int INDENT_SIZE = 2;
  
  Output a_output = Output(std::cout);
  
  void print(std::string name, const std::string type, bool highlight, bool indent_increase) {
    if (!indent_increase) {a_indent -= INDENT_SIZE;}
    print(name, type, highlight);
    if (indent_increase) {a_indent += INDENT_SIZE;}
  }
  
  void print(std::string name, const std::string type, bool highlight, Output::Color highlight_color = DEFAULT_HIGHLIGHT_COLOR) {
    std::string indent = std::string(a_indent, ' ');
    auto func = [&](std::ostream* out) {
      *out << indent << "[" + type + "] " << a_name << "::" << name << std::endl;
    };
    if (highlight) {
      a_output.print(highlight_color, func);
    } else {
      func(&std::cout);
    }
  }

public:
  
  Debug(std::string name) : a_name(name) {}

  inline void functionStart(std::string name, bool highlight = false) {
    if (enable) {
      print(name, "FUNCTION START", highlight, true);
    }
  }

  inline void functionEnd(std::string name, bool highlight = false) {
    if (enable) {
      print(name, "FUNCTION END", highlight, false);
    }
  }

  inline void debug(std::string name, bool highlight = false, Output::Color highlight_color = DEFAULT_HIGHLIGHT_COLOR) {
    if (enable) {
      print(name, "DEBUG", highlight, highlight_color);
    }
  }
  
};


#endif
