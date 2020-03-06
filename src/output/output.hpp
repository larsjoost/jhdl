#ifndef _OUTPUT_HPP
#define _OUTPUT_HPP

#include <cstdio>
#include <string>
#include <unistd.h>
#include <cstring>
#include <ostream>

class Output {

  const static int RESET = 0;
  const static int RED = 31;
  const static int GREEN = 32; 
  const static int YELLOW = 33; 
  const static int BLUE = 34; 
  const static int BRIGHT_RED = 91;
  const static int BRIGHT_BLUE = 94;

  bool m_bright_mode = true;
  
public:

  enum class Color {RESET, RED, GREEN, YELLOW, BLUE};
  
private:
  
  std::ostream* a_out;
  
  bool verbose = false;
  
  std::string colorCode(Color color);

  bool colorsSupported();
  
  bool isColorsSupported();

  int ConvertColor(Color color);

public:

  Output(std::ostream& out) {
    a_out = &out;
  };

  template<typename Func>
  void print(Color color, Func func) {
    *a_out << colorCode(color);
    func(a_out);
    *a_out << colorCode(Color::RESET);
  }

  void print(Color color, std::string text) {
    *a_out << colorCode(color) << text  << colorCode(Color::RESET);
  }

  void println(Color color, std::string text) {
    print(color, text);
    *a_out << std::endl;
  }
};

#endif
