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
#include <iostream>

#include "text.hpp"

namespace ast {

  Text::Text(bool verbose) : verbose(verbose) {}

  Text::Text(std::string& filename, char* text, bool caseSensitive, bool verbose) :
    verbose(verbose) {
    set(filename, text, caseSensitive);
  }

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
          lineStart = position + 1;
        }
        position++;
      }
    }
  }

  void Text::subString(Text& t, int size) {
    int s = remainingSize();
    assert(size <= s);
    t = *this;
    t.size = t.position + size;
  }
  
  void Text::set(std::string& filename, char* t, bool c) {
    this->filename = filename;
    text = t;
    position = 0;
    size = textSize = strlen(t);
    lineNumber = 0;
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
    return position - lineStart;
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
      bool done = false;
      int i = lineStart - position;
      do {
        char a = lookAhead(i++);
        if (a == '\n') {
          done = true;
        } else {
          if (a == '\t') {a = ' ';}
          s += a;
        }
      } while (!done);
    } catch (TextEof e) {
    }
    return s;
  }

  std::string Text::getCurrentLinePositionMarker() {
    std::string s = "";
    int size = getColumn();
    for (int i = 0; i < size; i++) {
      s +=  ' ';
    }
    s += "^";
    return s;
  }
  
  std::string Text::getFilename() {
    return filename;
  }

  void Text::print(std::ostream& output) {
    output << getFilename() << "(" << getLine() << "):" << std::endl;
    output << getCurrentLine() << std::endl;
    output << getCurrentLinePositionMarker() << std::endl;
  }
    
  
  void Text::debug(std::ostream& output) {
    output << "Text::position = " << position << std::endl;
    output << "Text::size = " << size << std::endl;
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

