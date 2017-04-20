#include "exceptions.hpp"

bool Exceptions::colorsSupported;
int Exceptions::numberOfErrors = 0;
int Exceptions::numberOfWarnings = 0;

std::string Exceptions::colorCode(int color) {
  return "\x1b[" + std::to_string(color) + "m";
}

void Exceptions::print(std::string severity, int color, std::string& message, ast::Text* text) {
  if (colorsSupported) {
    std::cerr << colorCode(color);
  }
  std::string location = text ?
    (" in " + text->getFilename() + " at " +
     std::to_string(text->getLine()) + ", " +
     std::to_string(text->getColumn())) : ""; 
  std::cerr << "#" << severity << location << ": " << message;
  if (text) {
    std::cerr << text->getCurrentLine() << std::endl;
    std::cerr << text->getCurrentLinePositionMarker() << std::endl;
  }
  if (colorsSupported) {
    std::cerr << colorCode(RESET);
  }
  std::cerr << std::endl;
}

void Exceptions::printError(std::string message, ast::Text* text) {
  numberOfErrors++;
  print("Error", RED, message, text);
}


void Exceptions::printWarning(std::string message, ast::Text* text) {
  numberOfWarnings++;
  print("Warning", BLUE, message, text);
}
