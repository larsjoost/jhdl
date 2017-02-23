
#ifndef AST_TEXT_H_
#define AST_TEXT_H_

#include <stdio.h>
#include <string>

namespace ast {

  class TextEof {};

  class Text {
    static char* text;
    static int textSize;
    int position = 0;
    int size = 0;
    int lineNumber = 0;
    int lineStart = 0;
    int columnNumber = 0;
    bool caseSensitive = false;

    char toLower(char c);

    void debug(const char* msg, char c = ' ');

    void printCurrentLine(FILE* output);
    void printCurrentLinePositionMarker(FILE* output);

  public:
    bool verbose = false;

    explicit Text() {};
    explicit Text(bool verbose);
    Text(char* text, bool caseSensitive = false, bool verbose = false);
    
    void set(char* text, bool caseSensitive = false);
    char lookAhead(int number);
    void incrementPosition(int size = 1);
    void subString(Text& t, int size);
    int remainingSize();
    int getLine();
    int getColumn();
    int getPosition();
    int getSize();
    int equals(Text& text);
    const std::string toString(bool setCase = false);
    
    std::string getCurrentLine();
    void printLinePosition(FILE* output = stdout);
    void print(FILE* output = stdout);
    void debug(FILE* output = stdout);
  };
  
}

#endif
