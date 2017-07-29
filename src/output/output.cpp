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

std::string Output::colorCode(int color) {
  if (isColorsSupported()) {
    return "\x1b[" + std::to_string(color) + "m";
  }
  return "";
}


