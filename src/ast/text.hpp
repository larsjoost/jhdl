
#ifndef AST_TEXT_H_
#define AST_TEXT_H_

#include <stdio.h>
#include <string>

namespace ast {

  class TextEof {};

  class Text {
    static char* text;
    static int textSize;
    int position;
    int size;
    int lineNumber;
    int lineStart;
    int columnNumber;
    int caseSensitive;

    char toLower(char c);

    void debug(const char* msg, char c = ' ');

    void printCurrentLine(FILE* output);
    void printCurrentLinePositionMarker(FILE* output);

  public:
    int verbose = 0;

    Text();
    Text(int verbose);
    Text(char* text, int caseSensitive = 0, int verbose = 0);
    
    void set(char* text, int caseSensitive = 0);
    char lookAhead(int number);
    void incrementPosition(int size = 1);
    void subString(Text& t, int size);
    int remainingSize();
    void advancePosition(int size);
    int getLine();
    int getColumn();
    int getPosition();
    int getSize();
    int equals(Text& text);
    const std::string toString();
    
    void printLinePosition(FILE* output = stdout);
    void print(FILE* output = stdout);
    void debug(FILE* output = stdout);
  };
  
}

#endif
