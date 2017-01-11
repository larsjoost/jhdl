
#ifndef AST_TEXT_H_
#define AST_TEXT_H_

#include <stdio.h>

namespace ast {

  class TextEof {};

  class Text {
    const char* text;
    int position;
    int size;
    int lineNumber;
    int lineStart;
    int columnNumber;
    int caseSensitive;

    char toLower(char c);

    int verbose = 0;

    void debug(const char* msg, char c = ' ');

    void printCurrentLine(FILE* output);
    void printCurrentLinePositionMarker(FILE* output);

  public:
    Text();
    Text(int verbose);
    Text(const char* text, int caseSensitive = 0, int verbose = 0);
    
    void set(const char* text, int caseSensitive = 0);
    char lookAhead(int number);
    void incrementPosition(int size = 1);
    Text subString(int size);
    int remainingSize();
    void advancePosition(int size);
    int getLine();
    int getColumn();
    int getPosition();
    void setSize(int size);
    int equals(Text& text);

    void printLinePosition(FILE* output);
    void print(FILE* output = stdout);
    void debug(FILE* output = stdout);
  };
  
}

#endif
