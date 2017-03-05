
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
    const char* filename;
    
    char toLower(char c);

    void debug(const char* msg, char c = ' ');

    void printCurrentLine(FILE* output, std::string head = "");
    void printCurrentLinePositionMarker(FILE* output, std::string head = "");

  public:
    bool verbose = false;

    explicit Text() {};
    explicit Text(bool verbose);
    Text(const char* filename, char* text, bool caseSensitive = false, bool verbose = false);
    
    void set(const char* filename, char* text, bool caseSensitive = false);
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
    void printException(const std::string &severity, const std::string &message, FILE* output = stderr, std::string head = "");
    void printLinePosition(FILE* output = stdout, std::string head = "");
    void print(FILE* output = stdout);
    void debug(FILE* output = stdout);
  };
  
}

#endif
