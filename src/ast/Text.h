/*
 * Text.h
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_AST_TEXT_H_
#define SRC_AST_TEXT_H_

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
    
    void get(Text& t);
    void set(const Text& text);
    void set(const char* text, int caseSensitive = 0);
    char lookAhead(int number);
    void incrementPosition();
    void advancePosition(int size);
    int getLine();
    int getColumn();
    int getPosition();
    void setSize(int size);
    int equals(Text& text);

    void printLinePosition(FILE* output);
  };
  
}

#endif /* SRC_AST_TEXT_H_ */
