#include "exceptions.hpp"

int Exceptions::numberOfErrors = 0;
int Exceptions::numberOfWarnings = 0;

std::string Exceptions::colorCode(int color) {
  return "\x1b[" + std::to_string(color) + "m";
}

void Exceptions::print(std::string severity, int color, std::string& message) {
  if (colorsSupported) {
    std::cerr << colorCode(color);
  }
  std::cerr << "#" << severity << ": " << message;
  if (colorsSupported) {
    std::cerr << colorCode(RESET);
  }
  std::cerr << std::endl;
}

void Exceptions::printError(std::string message, ast::Text* text) {
  numberOfErrors++;
  if (text) {
    text->printException("error", message);
  } else {
    print("Error", RED, message);
  }
}


void Exceptions::printWarning(std::string message, ast::Text* text) {
  numberOfWarnings++;
  if (text) {
    text->printException("warning", message, std::cout, "// ");
  } else {
    print("Warning", BLUE, message);
  }
}
