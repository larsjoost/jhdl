
#ifndef AST_SCANNER_HPP_
#define AST_SCANNER_HPP_

#include <string>
#include <exception>

#include "text.hpp"
#include "list.hpp"
#include "identifier.hpp"

namespace ast {

  class UnexpectedToken {
  public:
    Text text;
    UnexpectedToken(Text t) {
      text = t;
    }
  };
  class ExpectFailed : public std::exception  {};

  class NoWhiteSpace {
  public:
    char letter;
    NoWhiteSpace(char c) {
      letter = c;
    }
  };
  class TokenNotAccepted : public std::exception {};
  class FileNotFound : public std::exception  {};
  class CriticalError : public std::exception  {};
  class SyntaxError : public std::exception  {};
  
  class Scanner {

  private:
    Text* text;
    const char* filename = "";
    
    int number_of_errors = 0;
    
    void print(const char* severity, const char* format, ...);

    void debug(const char* msg, const char* a = '\0');

    int caseSensitive;
    
    int verbose;

    int compare(char l, char r);
    
  public:

    Scanner(int caseSensitive, int verbose = 0);
        
    void setText(const char* s);
    void loadFile(const char* filename);
    
    int accept(const char *text);
    int optional(const char* text);
    int expect(const char* text);
    int expect(Text& text);
    int match(const char* text);
    char lookAhead(int number);
    void eat(Text& text, int size);
    void incrementPosition(int size = 1);
    
    int isWhiteSpace();
    int skipWhiteSpace();
    int skipOneOrMoreWhiteSpaces();
    int skipUntilEndOfLine();
    
    int getPosition();
    
    void getText(Text& t);
    void setText(const Text& t);
    
    void warning(const char* format, ...);
    void error(const char* format, ...);
    void critical(const char* format, ...);

    int getNumberOfErrors();

  };  



}

#endif

    
