#include "exceptions.hpp"

int Exceptions::m_number_of_errors = 0;
int Exceptions::m_number_of_warnings = 0;

void Exceptions::print(std::string severity, Output::Color color, std::string& message, ast::Text* text) {
  auto func = [&](std::ostream* out) {
    std::string location = text ?
    (" in " + text->getFilename() + "(" +
     std::to_string(text->getLine()) + ")") : ""; 
    *out << "#" << severity << location << ": " << message << std::endl;
    if (text) {
      *out << text->getCurrentLine() << std::endl;
      *out << text->getCurrentLinePositionMarker() << std::endl;
    }
  };
  m_output.print(color, func);
}

void Exceptions::printInternal(std::string message, ast::Text* text) {
  print("Internal", Output::Color::RED, message, text);
}

void Exceptions::printNote(std::string message, ast::Text* text) {
  if (verbose) {
    print("Note", Output::Color::YELLOW, message, text);
  }
}

void Exceptions::printError(std::string message, ast::Text* text, const char* file_name, int line_number) {
  m_number_of_errors++;
  if (file_name) {
    message = std::string(file_name) + "(" + std::to_string(line_number) + "): " + message;
  }
  print("Error", Output::Color::RED, message, text);
}

void Exceptions::printError(std::string message, const char* file_name, int line_number) {
  printError(message, NULL, file_name, line_number);
}
void Exceptions::printWarning(std::string message, ast::Text* text) {
  m_number_of_warnings++;
  if (a_display_warnings) {
    print("Warning", Output::Color::YELLOW, message, text);
  }
}
  
