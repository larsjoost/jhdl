
#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <string>

#include "Text.h"
#include "identifier.h"

namespace ast {

  class UnexpectedToken {
  public:
    const char* text;
    UnexpectedToken(const char* t) {
      text = t;
    }
  };
  class NoWhiteSpace {
  public:
    char letter;
    NoWhiteSpace(char c) {
      letter = c;
    }
  };
  class TokenNotAccepted {};
  class FileNotFound {};
  class CriticalError {};
  
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
    int match(const char* text);
    char lookAhead(int number);
    void eat(Text& t, int size);
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

#endif /* VHDLSCANNER_HPP_ */
