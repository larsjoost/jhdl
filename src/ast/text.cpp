/*
 * Text.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include <cstring>
#include <cctype>
#include <cassert>
#include <stdio.h>
#include <algorithm>

#include "text.hpp"

namespace ast {

  char* Text::text = NULL;
  int Text::textSize = 0;

  Text::Text() {
  };

  Text::Text(int v) {
    verbose = v;
  };

  Text::Text(char* text, bool caseSensitive, int v) {
    verbose = v;
    set(text, caseSensitive);
  };

  void Text::debug(const char* msg, char a) {
    if (verbose) {
      printf("ast::Text.cpp: %s = %c\n", msg, a);
    }
  }

  void Text::incrementPosition(int s) {
    debug("incrementPosition", lookAhead(0));
    if ((position + s) >= textSize) {
      throw TextEof();
    } else {
      for (int i=0; i<s; i++) {
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
  }

  void Text::subString(Text& t, int s) {
    assert(s <= remainingSize());
    t = *this;
    t.size = t.position + s;
  }
  
  void Text::advancePosition(int n) {
    for (int i = 0; i < n; i++) {
      incrementPosition();
    }
  }

  void Text::set(char* t, bool c) {
    text = t;
    position = 0;
    size = textSize = strlen(t);
    lineNumber = 0;
    columnNumber = 0;
    lineStart = 0;
    caseSensitive = c;
  }

  char Text::lookAhead(int n) {
    int i = position + n;
    if (i >= textSize) {
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

  int Text::getSize() {
    return textSize;
  }
  
  char Text::toLower(char a) {
    if (!caseSensitive) {
      return tolower(a);
    }
    return a;
  }

  int Text::remainingSize() {
    return size - position;
  }
  
  int Text::equals(Text& t) {
    int s = remainingSize();
    if (s == t.remainingSize()) {
      for (int i=0; i<s; i++) {
        if (toLower(lookAhead(i)) != toLower(t.lookAhead(i))) {
          return 0;
        }
      }
    } else {
      return 0;
    }
    return 1;
  }

  std::string Text::getCurrentLine() {
    std::string s = "";
    try {
      char a;
      bool done = false;
      int i = lineStart - position;
      do {
        a = lookAhead(i++);
        if (a == '\n') {
          done = true;
        } else {
          s += a;
        }
      } while (!done);
    } catch (TextEof e) {
    }
    return s;
  }
  
  void Text::printCurrentLine(FILE* output) {
    fprintf(output, getCurrentLine().c_str());
    fprintf(output, "\n");
  }

  void Text::printCurrentLinePositionMarker(FILE* output) {
    for (int i = 0; i < (position - lineStart); i++) {
      fputc(' ', output);
    }
    fprintf(output, "^\n");
  }
  
  void Text::printLinePosition(FILE* output) {
    printCurrentLine(output);
    printCurrentLinePositionMarker(output);
  }

  void Text::print(FILE* output) {
    for (int i = 0; i < (size - position); i++) {
      fputc(lookAhead(i), output);
    }
  }

  void Text::debug(FILE* output) {
    fprintf(output, "Text::position = %u\n", position);
    fprintf(output, "Text::size = %u\n", size);
  }

  const std::string Text::toString(bool setCase) {
    int size = remainingSize();
    std::string s;
    for (int i = 0; i < size; i++) {
      char c = lookAhead(i);
      if (setCase && !caseSensitive) {
	c = toupper(c);
      }
      s += c;
    }
    return s;
  }

}

