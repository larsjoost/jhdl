 
#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>

#include "scanner.hpp"
#include "text.hpp"

namespace ast {

  Scanner::Scanner(int c, int v) {
    caseSensitive = c;
    verbose = v;
    text = new Text(v);
  }

  void Scanner::setText(const char* s) {
    text->set(s);
  }

  void Scanner::loadFile(const char* filename) {
    if (verbose) {
      printf("Reading file %s\n", filename);
    }
    this->filename = std::string(filename);
    FILE *f = fopen(filename, "rb");
    if (f != NULL) {
      fseek(f, 0, SEEK_END);
      long fsize = ftell(f);
      fseek(f, 0, SEEK_SET);  //same as rewind(f);

      char *string = (char *)malloc(fsize + 1);
      fread(string, fsize, 1, f);
      fclose(f);

      string[fsize] = 0;
      text->set(string);
    } else {
      throw FileNotFound();
    }
  }

  int Scanner::compare(char l, char r) {
    if (!caseSensitive) {
      l = tolower(l);
      r = tolower(r);
    }
    return (l == r);
  }
  
  int Scanner::match(const char* t) {
    DEBUG("match " + std::string(t));
    int len = strlen(t);
    try {
      for (int i=0; i<len; i++) {
        char a = text->lookAhead(i);
        if (!compare(a, t[i])) {
          return 0;
        }
      }
    } catch (...) {return 0;}
    return len;
  }

  int Scanner::optional(const char* t) {
    DEBUG("optional " + std::string(t));
    int len = match(t);
    if (len > 0) {
      text->advancePosition(len);
    }
    return len;
  }
  
  int Scanner::accept(const char* t) {
    DEBUG("accept" + std::string(t));
    int len = optional(t);
    if (len == 0) {
      throw TokenNotAccepted();
    }
    return len;
  }

  int Scanner::expect(const char* t) {
    DEBUG("expect" + std::string(t));
    int len = optional(t);
    if (len == 0) {
      error("Expected '" + std::string(t));
    }
    return len;
  }

  int Scanner::expect(Text& t) {
    int size = t.remainingSize();
    Text a = text->subString(size);
    if (!a.equals(t)) {
      error("Expected '" + t.toString() + "'");
    } else {
      incrementPosition(size);
    };
  }

  char Scanner::lookAhead(int n) {
    return text->lookAhead(n);
  }

  void Scanner::incrementPosition(int size) {
    text->incrementPosition(size);
  }

  void Scanner::eat(Text& t, int size) {
    t = text->subString(size);
    incrementPosition(size);
  }
  
  void Scanner::getText(Text& t) {
    t = *text;
  }

  void Scanner::setText(const Text& t) {
    *text = t;
  }

  int Scanner::getPosition() {
    return text->getPosition();
  }

  int Scanner::isWhiteSpace() {
    char a = text->lookAhead(0);
    if (a == ' ' || a == '\t' || a == '\n' || a == '\r') {
      return 1;
    }
    return 0;
  }

  int Scanner::skipWhiteSpace() {
    DEBUG("skipWhiteSpace");
    int i = 0;
    while (isWhiteSpace()) {
      text->incrementPosition();
      i++;
    }
    return i;
  }

  int Scanner::skipOneOrMoreWhiteSpaces() {
    DEBUG("skipOneOrMoreWhiteSpace");
    if (!isWhiteSpace()) {
      error("Expected white-space");
    }
    return skipWhiteSpace();
  }
  
  int Scanner::skipUntilEndOfLine() {
    int i = 0;
    while (text->lookAhead(0) != '\n') {
      text->incrementPosition();
      i++;
    }
    return i;
  }

  void Scanner::print(const std::string &severity, const std::string &text) {
    std::cerr << severity << " in file " << filename << " at "
	      << std::to_string(this->text->getLine()) << ", "
	      << std::to_string(this->text->getColumn()) << ": "
	      << text << std::endl;
    this->text->printLinePosition(stderr);
  }

  void Scanner::error(const std::string &s)
  {
    number_of_errors++;
    print("error", s);
    throw SyntaxError();
  }

  void Scanner::warning(const std::string &s) {
    print("warning", s);
  }
  
  void Scanner::critical(const std::string &s) {
    print("critical", s);
    throw CriticalError();
  }

  int Scanner::getNumberOfErrors() {
    return number_of_errors;
  }

  
}
