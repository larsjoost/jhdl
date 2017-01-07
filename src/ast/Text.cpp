/*
 * Text.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include <cstring>
#include <cctype>
#include <stdio.h>

#include "Text.h"

namespace ast {

  Text::Text() {
  };

  Text::Text(int v) {
    verbose = v;
  };

  Text::Text(const char* text, int caseSensitive, int v) {
    verbose = v;
    set(text, caseSensitive);
  };

  void Text::debug(const char* msg, char a) {
    if (verbose) {
      printf("ast::Text.cpp: %s = %c\n", msg, a);
    }
  }

  void Text::incrementPosition() {
    debug("incrementPosition", lookAhead(0));
    if ((position + 1) >= size) {
      throw TextEof();
    } else {
      if (text[position] == '\n') {
        lineNumber++;
        columnNumber = 0;
        lineStart = position + 1;
      } else {
        columnNumber++;
      }
      position++;
    }
  }

  void Text::advancePosition(int n) {
    for (int i = 0; i < n; i++) {
      incrementPosition();
    }
  }

  void Text::get(Text& t) {
    t.text = text;
    t.position = position;
    t.size = size;
    t.lineNumber = lineNumber;
    t.columnNumber = columnNumber;
    t.lineStart = lineStart;
    t.caseSensitive = caseSensitive;
  }

  void Text::set(const Text& t) {
    text = t.text;
    position = t.position;
    size = t.size;
    lineNumber = t.lineNumber;
    columnNumber = t.columnNumber;
    lineStart = t.lineStart;
    caseSensitive = t.caseSensitive;
  }

  void Text::set(const char* t, int c) {
    text = t;
    position = 0;
    size = strlen(t);
    lineNumber = 0;
    columnNumber = 0;
    lineStart = 0;
    caseSensitive = c;
  }

  char Text::lookAhead(int n) {
    int i = position + n;
    if (i >= size) {
      throw TextEof();
    }
    return text[i];
  }

  int Text::getLine() {
    return lineNumber;
  }

  int Text::getColumn() {
    return columnNumber;
  }

  int Text::getPosition() {
    return position;
  }

  void Text::setSize(int s) {
    size = s;
  }

  char Text::toLower(char a) {
    if (!caseSensitive) {
      return tolower(a);
    }
    return a;
  }
  
  int Text::equals(Text& t) {
    if (size == t.size) {
      for (int i=0; i<size; i++) {
        if (toLower(lookAhead(i)) != toLower(t.lookAhead(i))) {
          return 0;
        }
      }
    } else {
      return 0;
    }
    return 1;
  }

  void Text::printCurrentLine(FILE* output) {
   try {
      char a;
      int i = lineStart - position;
      fprintf(output, "%u: ", lineNumber);
      do {
        a = lookAhead(i++);
        fputc(a, output);
      } while (a != '\n');
    } catch (TextEof e) {
    }
  }

  void Text::printCurrentLinePositionMarker(FILE* output) {
    fprintf(output, "   ");
    int i = lineNumber;
    while (i > 0) {
      i /= 10;
      fputc(' ', output);
    }
    for (i = 0; i < (position - lineStart); i++) {
      fputc(' ', output);
    }
    fprintf(output, "^\n");
  }
  
  void Text::printLinePosition(FILE* output) {
    printCurrentLine(output);
    printCurrentLinePositionMarker(output);
  }

  void Text::print(FILE* output) {
    for (int i = 0; i < size; i++) {
      fputc(lookAhead(i), output);
    }
  }
  
}
