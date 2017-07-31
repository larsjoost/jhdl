#include <iostream>

#include "debug.hpp"

EnableDebug::EnableDebug(std::string name) {
  a_name = name;
}

void EnableDebug::debug(std::string name, bool highlight, int highlight_color) {
  print(a_function_name + ": " + name, "DEBUG", highlight, highlight_color);
}

void EnableDebug::print(std::string name, const std::string type, bool highlight, bool indent_increase) {
  if (!indent_increase) {a_indent -= INDENT_SIZE;}
  print(name, type, highlight);
  if (indent_increase) {a_indent += INDENT_SIZE;}
}
void EnableDebug::print(std::string name, const std::string type, bool highlight, int highlight_color) {
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

void EnableDebug::functionStart(std::string name, bool highlight) {
  print(name, "FUNCTION START", highlight, true);
  a_function_name = name;
}

void EnableDebug::functionEnd(std::string name, bool highlight) {
  print(name, "FUNCTION END", highlight, false);
}


