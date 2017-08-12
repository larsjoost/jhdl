#include "exceptions.hpp"

int Exceptions::a_number_of_errors = 0;
int Exceptions::a_number_of_warnings = 0;

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
  a_number_of_errors++;
  print("Error", Output::Color::RED, message, text);
}

void Exceptions::printWarning(std::string message, ast::Text* text) {
  a_number_of_warnings++;
  if (a_display_warnings) {
    print("Warning", Output::Color::YELLOW, message, text);
  }
}
  
