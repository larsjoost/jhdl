 
#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "scanner.hpp"
#include "text.hpp"

namespace ast {

  Scanner::Scanner(int c, int v) {
    caseSensitive = c;
    verbose = v;
    text = new Text(v);
  }

  void Scanner::debug(const char* msg, const char* a) {
    if (verbose) {
      printf("ast::Scanner.cpp: %s = %s\n", msg, a);
    }
  }
  
  void Scanner::setText(const char* s) {
    text->set(s);
  }

  void Scanner::loadFile(const char* filename) {
    if (verbose) {
      printf("Reading file %s\n", filename);
    }
    this->filename = filename;
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
    debug("match", t);
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
    debug("optional", t);
    int len = match(t);
    if (len > 0) {
      text->advancePosition(len);
    }
    return len;
  }
  
  int Scanner::accept(const char* t) {
    debug("accept", t);
    int len = optional(t);
    if (len == 0) {
      throw TokenNotAccepted();
    }
    return len;
  }

  int Scanner::expect(const char* t) {
    debug("expect", t);
    int len = optional(t);
    if (len == 0) {
      error("Expected '%s'\n", t);
      throw UnexpectedToken(t);
    }
    return len;
  }

  int Scanner::expect(Text& t) {
    int size = t.remainingSize();
    Text a = text->subString(size);
    if (!a.equals(t)) {
      error("Expected '%s', but found '%s'", t.toString(), a.toString());
      throw UnexpectedToken(t);
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
    debug("skipWhiteSpace");
    int i = 0;
    while (isWhiteSpace()) {
      text->incrementPosition();
      i++;
    }
    return i;
  }

  int Scanner::skipOneOrMoreWhiteSpaces() {
    debug("skipOneOrMoreWhiteSpace");
    if (!isWhiteSpace()) {
      error("Expected white-space");
      throw NoWhiteSpace(text->lookAhead(0));
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

  void Scanner::print(const char* severity, const char* format, ...) {
    fprintf(stderr, "#%s in file %s at %i, %i: ", severity,
            filename, text->getLine(), text->getColumn());
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    fprintf(stderr, "\n");
    text->printLinePosition(stderr);
  }

  void Scanner::error(const char* format, ...)
  {
    number_of_errors++;
    va_list argptr;
    va_start(argptr, format);
    print("error", format, argptr);
    va_end(argptr);
  }

  void Scanner::warning(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    print("warning", format, argptr);
    va_end(argptr);
  }
  
  void Scanner::critical(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    print("critical", format, argptr);
    va_end(argptr);
    throw CriticalError();
  }

  int Scanner::getNumberOfErrors() {
    return number_of_errors;
  }

  
}
