#include "exceptions.hpp"

int Exceptions::numberOfErrors = 0;
int Exceptions::numberOfWarnings = 0;

std::string Exceptions::colorCode(int color) {
  if (colorsSupported) {
    return "\x1b[" + std::to_string(color) + "m";
  }
  return "";
}

void Exceptions::print(std::string severity, int color, std::string& message, ast::Text* text) {
  std::cerr << colorCode(color);
  std::string location = text ?
    (" in " + text->getFilename() + " at " +
     std::to_string(text->getLine()) + ", " +
     std::to_string(text->getColumn())) : ""; 
  std::cerr << "#" << severity << location << ": " << message << std::endl;
  if (text) {
    std::cerr << text->getCurrentLine() << std::endl;
    std::cerr << text->getCurrentLinePositionMarker() << std::endl;
  }
  std::cerr << colorCode(RESET);
}

void Exceptions::printInternal(std::string message, ast::Text* text) {
  print("Internal", RED, message, text);
}

void Exceptions::printError(std::string message, ast::Text* text) {
  numberOfErrors++;
  print("Error", RED, message, text);
}

void Exceptions::printWarning(std::string message, ast::Text* text) {
  numberOfWarnings++;
  print("Warning", BLUE, message, text);
}
