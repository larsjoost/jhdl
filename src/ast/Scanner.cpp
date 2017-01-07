/*
 * VhdlScanner.cpp
 *
 *  Created on: 4 Jan 2017
 *      Author: lars_
 */

#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Scanner.h"
#include "Text.h"

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
    printf("Reading file %s\n", filename);
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
        if (!compare(text->lookAhead(i), t[i])) {
          return 0;
        }
      }
    } catch (...) {return 0;}
    return len;
  }

  int Scanner::accept(const char* t) {
    debug("accept", t);
    int len = match(t);
    text->advancePosition(len);
    return len;
  }

  int Scanner::optional(const char* t) {
    debug("optional", t);
    return accept(t);
  }

  int Scanner::expect(const char* t) {
    debug("expect", t);
    int len = accept(t);
    if (!len) {
      throw UnexpectedToken(t);
    }
    return len;
  }

  char Scanner::lookAhead(int n) {
    return text->lookAhead(n);
  }

  void Scanner::incrementPosition() {
    text->incrementPosition();
  }
    
  void Scanner::getText(Text& t) {
    text->get(t);
  }

  void Scanner::setText(const Text& t) {
    text->set(t);
  }

  int Scanner::getPosition() {
    return text->getPosition();
  }

  int Scanner::isWhiteSpace() {
    char a = text->lookAhead(0);
    if (a == ' ' || a == '\t' || a == '\n') {
      return 1;
    }
    return 0;
  }

  int Scanner::skipWhiteSpace() {
    int i = 0;
    try {
      if (isWhiteSpace()) {
        text->incrementPosition();
        i++;
      }
    } catch(...) {};
    return i;
  }

  int Scanner::skipOneOrMoreWhiteSpaces() {
    if (!isWhiteSpace()) {
      throw NoWhiteSpace(text->lookAhead(0));
    }
    return skipWhiteSpace();
  }
  
  int Scanner::skipUntilEndOfLine() {
    int i = 0;
    try {
      while (text->lookAhead(0) != '\n') {
        text->incrementPosition();
        i++;
      }
    } catch(...) {};
    return i;
  }

  void Scanner::print(const char* severity, const char* format, ...) {
    fprintf(stderr, "#%s in file %s at %i, %i: ", severity,
            filename, text->getLine(), text->getColumn());
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    text->printLinePosition(stderr);
  }

  void Scanner::error(const char* format, ...)
  {
    number_of_errors++;
    va_list argptr;
    va_start(argptr, format);
    printf("error", format, argptr);
    va_end(argptr);
  }

  void Scanner::warning(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    printf("warning", format, argptr);
    va_end(argptr);
  }
  
  void Scanner::critical(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    printf("critical", format, argptr);
    va_end(argptr);
    throw CriticalError();
  }

  int Scanner::getNumberOfErrors() {
    return number_of_errors;
  }
  
}
