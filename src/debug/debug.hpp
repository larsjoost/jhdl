#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <string>
#include <iostream>
#include <cassert>

#include <boost/core/demangle.hpp>

#include "../output/output.hpp"

template<bool enable>
class Debug {

protected:
  
  const static Output::Color DEFAULT_HIGHLIGHT_COLOR = Output::Color::GREEN;

  std::string m_name;
  std::string m_subname;
  bool m_verbose;
  static int m_indent;
  const int INDENT_SIZE = 2;
  
  Output m_output = Output(std::cout);
  
  void print(std::string name, std::string type, bool highlight, bool indent_increase,
	     const char* file_name = NULL, int line_number = -1) {
    if (!indent_increase) {m_indent -= INDENT_SIZE;}
    print(name, type, highlight, DEFAULT_HIGHLIGHT_COLOR, file_name, line_number);
    if (indent_increase) {m_indent += INDENT_SIZE;}
  }
  
  void print(std::string name, std::string type, bool highlight, Output::Color highlight_color = DEFAULT_HIGHLIGHT_COLOR,
	     const char* file_name = NULL, int line_number = -1) {
    std::string indent = std::string(m_indent, ' ');
    auto func = [&](std::ostream* out) {
		  *out << indent << "[" + type + "] " <<
		    (file_name ? std::string(file_name) + "(" + std::to_string(line_number) + "): " : "") <<
		    m_name << (m_subname.empty() ? "" : "(" + m_subname + ")") << "::" << name << std::endl;
    };
    if (highlight) {
      m_output.print(highlight_color, func);
    } else {
      func(&std::cout);
    }
  }

public:

  Debug(const char* name) : m_name(name) { m_verbose = enable; };
  
  template <class T>
  Debug(T* instance, std::string subname = "") : m_name(boost::core::demangle(typeid(*instance).name())), m_subname(subname) {
    m_verbose = enable;
  }

  inline void functionStart(std::string name, bool highlight = false, const char* file_name = NULL, int line_number = -1) {
    if (isVerbose()) {
      print(name, "FUNCTION START", highlight, true, file_name, line_number);
    }
  }

  inline void functionEnd(std::string name, bool highlight = false) {
    if (isVerbose()) {
      print(name, "FUNCTION END", highlight, false);
    }
  }

  inline void debug(std::string name, bool highlight = false, Output::Color highlight_color = DEFAULT_HIGHLIGHT_COLOR,
		    const char* file_name = NULL, int line_number = 0) {
    if (isVerbose()) {
      print(name, "DEBUG", highlight, highlight_color, file_name, line_number);
    }
  }

  inline void debug(std::string name, const char* file_name, int line_number) {
    debug(name, false, DEFAULT_HIGHLIGHT_COLOR, file_name, line_number);
  }

  inline bool isVerbose() {
    return enable && m_verbose;
  }

  void setVerbose(bool verbose) {
    m_verbose = verbose;
  }
  
};

template<bool enable>
int Debug<enable>::m_indent = 0;

#endif
