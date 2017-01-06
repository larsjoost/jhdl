
#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <string>

#include "Text.h"

namespace ast {

  class Identifier {
  public:
    Text text;
    int equals(Identifier& t);
  };

  class UnexpectedToken {};
  class TokenNotAccepted {};
  
  class Scanner {

  private:
    Text* text;
    const char* filename = "";
    
    int number_of_errors = 0;
    
    void print(const char* severity, const char* msg);
public:

    Scanner();
        
    void setText(const char* s);
    void loadFile(const char* filename);
    
    int accept(const char *text);
    int optional(const char* text);
    int expect(const char* text);
    int match(const char* text);
    char lookAhead(int number);
    void incrementPosition();
    
    int isWhiteSpace();
    int skipWhiteSpace();
    int skipUntilEndOfLine();
    
    int getPosition();
    
    void getText(Text& t);
    void setText(const Text& t);
    
    void warning(const char* msg);
    void error(const char* msg);
    void critical(const char* msg);
    
    int getNumberOfErrors();
  };
  
}

#endif /* VHDLSCANNER_HPP_ */
