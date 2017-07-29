#include <iostream>

#include "debug.hpp"

Debug::Debug(std::string name, bool verbose) {
  a_name = name;
  a_verbose = verbose;
}

void Debug::debug(std::string name, bool highlight) {
  print(a_function_name + ": " + name, "DEBUG", highlight);
}

void Debug::print(std::string name, const std::string type, bool highlight, bool indent_increase) {
  if (!indent_increase) {a_indent -= INDENT_SIZE;}
  print(name, type, highlight);
  if (indent_increase) {a_indent += INDENT_SIZE;}
}
void Debug::print(std::string name, const std::string type, bool highlight) {
  if (a_verbose) {
    std::string indent = std::string(a_indent, ' ');
    auto func = [&](std::ostream* out) {
      *out << indent << "[" + type + "] " << a_name << "::" << name << std::endl;
    };
    if (highlight) {
      a_output.print(Output::RED, func);
    } else {
      func(&std::cout);
    }
  }
}

void Debug::functionStart(std::string name, bool highlight) {
  print(name, "FUNCTION START", highlight, true);
  a_function_name = name;
}

void Debug::functionEnd(std::string name, bool highlight) {
  print(name, "FUNCTION END", highlight, false);
}


