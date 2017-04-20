
#ifndef AST_TEXT_H_
#define AST_TEXT_H_

#include <stdio.h>
#include <string>
#include <iostream>

namespace ast {

  class TextEof {};

  class Text {
    char* text;
    int textSize;
    int position = 0;
    int size = 0;
    int lineNumber = 1;
    int lineStart = 0;
    bool caseSensitive = false;
    std::string filename;
    
    char toLower(char c);

    void debug(const char* msg, char c = ' ');

  public:
    bool verbose = false;

    explicit Text() {};
    explicit Text(bool verbose);
    Text(std::string& filename, char* text, bool caseSensitive = false, bool verbose = false);

    std::string getFilename();
    void set(std::string& filename, char* text, bool caseSensitive = false);
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
    std::string getCurrentLinePositionMarker();

    void print(std::ostream& output = std::cout);
    void debug(std::ostream& output = std::cout);
  };
  
}

#endif
