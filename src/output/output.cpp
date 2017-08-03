#include <cassert>

#include "output.hpp"

bool Output::colorsSupported() {
  if (isatty(STDERR_FILENO)) {
    char* term = getenv("TERM");
    if (term && strcmp(term, "dumb")) {
      return true;
    }
  }
  return false;
}

bool Output::isColorsSupported() {
  static bool s = colorsSupported();
  return s;
}

int Output::ConvertColor(Color color) {
  switch (color) {
  case Color::RESET: return RESET;
  case Color::RED: return RED;
  case Color::GREEN: return GREEN;
  case Color::YELLOW: return YELLOW;
  case Color::BLUE: return BLUE;
  }
  assert(false);
}

std::string Output::colorCode(Color color) {
  if (isColorsSupported()) {
    return "\x1b[" + std::to_string(ConvertColor(color)) + "m";
  }
  return "";
}


