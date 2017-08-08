#include "exceptions.hpp"

int Exceptions::numberOfErrors = 0;
int Exceptions::numberOfWarnings = 0;

void Exceptions::print(std::string severity, Output::Color color, std::string& message, ast::Text* text) {
  auto func = [&](std::ostream* out) {
    std::string location = text ?
    (" in " + text->getFilename() + " at " +
     std::to_string(text->getLine()) + ", " +
     std::to_string(text->getColumn())) : ""; 
    *out << "#" << severity << location << ": " << message << std::endl;
    if (text) {
      *out << text->getCurrentLine() << std::endl;
      *out << text->getCurrentLinePositionMarker() << std::endl;
    }
  };
  a_output.print(color, func);
}

void Exceptions::printInternal(std::string message, ast::Text* text) {
  print("Internal", Output::Color::RED, message, text);
}

void Exceptions::printNote(std::string message, ast::Text* text) {
  if (verbose) {
    print("Note", Output::Color::YELLOW, message, text);
  }
}

void Exceptions::printError(std::string message, ast::Text* text) {
  numberOfErrors++;
  print("Error", Output::Color::RED, message, text);
}

void Exceptions::printWarning(std::string message, ast::Text* text) {
  numberOfWarnings++;
  if (a_display_warnings) {
    print("Warning", Output::Color::YELLOW, message, text);
  }
}
  
