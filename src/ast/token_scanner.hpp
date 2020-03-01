
#ifndef AST_TOKENSCANNER_HPP_
#define AST_TOKENSCANNER_HPP_

#include <exception>
#include <fstream>
#include <string>
#include <cstring>
#include <array>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "text.hpp"
#include "list.hpp"
#include "identifier.hpp"

#include "../exceptions/exceptions.hpp"
#include "../debug/debug.hpp"

namespace ast {

  class SyntaxError : public std::exception {};
  class CriticalError : public std::exception {};
  class FileNotFound : public std::exception {};
  class TokenEof : public std::exception {};
  class TokenNotAccepted : public std::exception {};

  enum TokenType {
    TOKEN_KEYWORD,
    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_STRING,
    TOKEN_CHARACTER,
    TOKEN_IDENTIFIER,
    TOKEN_SPECIAL_CHARACTER
  };

  template <class ApplicationSpecificScanner>
  class TokenScanner {

    Debug<false> debug;
    Exceptions exceptions;
    
    bool skippedWhiteSpace = false;
  public:

    TokenScanner() : debug("ast::TokenScanner") {}
    
    using Keyword = typename ApplicationSpecificScanner::Keyword;
    
    struct Token {
      TokenType type;
      Keyword keyword;
      Text text;
      bool skippedWhiteSpace;
    };

  private:
    
    std::string filename;
    Text text;

    struct TokenInfo {
      Token** tokens;
      int position;
      int size;
    };

    TokenInfo tokenInfo;
    
    ApplicationSpecificScanner applicationSpecificScanner;
    
    int number_of_errors = 0;
    
  protected:
    
    bool verbose = false;

  private:
    
    bool isKeyword(Text& t, Keyword& k);
    Token* acceptString();
    Token* acceptCharacter();
    Token* acceptSpecialCharacter();
    Token* acceptIdentifier();
    Token* acceptNumber();
    bool tokenize();

    int compare(char l, char r);
    bool match(Text& t, const char* c);
    bool optional(Text& t, const char* c);

    int getPosition();

    void incrementPosition(int size = 1);
    bool add(Token* token);
    
  public:

    void setVerbose(bool verbose);
        
    int isWhiteSpace();
    int skipWhiteSpace();
    int skipUntilEndOfLine();

    bool accept(const char* c);
    
    void setText(std::string& filename, char* s);
    void loadFile(std::string& filename);
    
    void getText(Text& t);
    void setText(const Text& t);
    
    void warning(const std::string &s);
    void error(const std::string &s);
    void critical(const std::string &s);

    int getNumberOfErrors();

    Token* tokenLookAhead(int number);
    void nextToken(int number = 1);
    int getTokenPosition();
    void setTokenPosition(int p);
    
    std::string& getFilename();
    
    std::string toString(Token* t);
    std::string toString(Keyword k);
    std::string toString(TokenType k);
    std::string toString(Text* t);

    std::string currentTokenToString();
    
  };  


  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::setVerbose(bool verbose) {
    this->verbose = verbose;
    text.verbose = verbose;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::compare(char l, char r) {
    if (!ApplicationSpecificScanner::CASE_SENSITIVE) {
      l = tolower(l);
      r = tolower(r);
    }
    return (l == r);
  }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::match(Text& t, const char* a) {
    debug.debug("match " + std::string(a));
    int len = strlen(a);
    for (int i=0; i<len; i++) {
      char c = text.lookAhead(i);
      if (!compare(c, a[i])) {
        return false;
      }
    }
    text.subString(t, len);
    return true;
  }

  template <class ApplicationSpecificScanner>
  inline bool TokenScanner<ApplicationSpecificScanner>::optional(Text& t, const char* c) {
    debug.debug("optional " + std::string(c));
    bool result; 
    if (result = match(t, c)) {
      incrementPosition(t.remainingSize());
    }
    return result;
  }
   
  template <class ApplicationSpecificScanner>
  inline bool TokenScanner<ApplicationSpecificScanner>::accept(const char* c) {
     debug.debug("accept" + std::string(c));
     Text t;
     if (!optional(t, c)) {
       throw TokenNotAccepted();
     }
     return true;
   }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::add(Token* token) {
    if (token) {
      debug.debug("Added token " + token->text.toString());
      token->skippedWhiteSpace = skippedWhiteSpace;
      skippedWhiteSpace = false;
      tokenInfo.tokens[tokenInfo.size] = token;
      tokenInfo.size++;
      text.incrementPosition(token->text.remainingSize());
      return true;
    }
    return false;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptString() {
    debug.functionStart("acceptString");
    Token* t = NULL;
    int i=0;
    if (text.lookAhead(0) == '\"') {
      i=1;
    } else if (text.lookAhead(1) == '\"' &&
               (text.lookAhead(0) == 'x' || text.lookAhead(0) == 'X' ||
                text.lookAhead(0) == 'b' || text.lookAhead(0) == 'B' ||
                text.lookAhead(0) == 'o' || text.lookAhead(0) == 'O')) {
      i=2;
    }
    if (i > 0) {
      bool string_end_found = false;
      do {
        if (text.lookAhead(i++) == '\"') {
          if (text.lookAhead(i) == '\"') {
            i++;
          } else {
            string_end_found = true;
          }
        }
      } while (!string_end_found);
      t = new Token();
      t->type = TOKEN_STRING;
      text.subString(t->text, i);
    }
    debug.functionEnd("acceptString");
    return t;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptCharacter() {
    if (text.lookAhead(0) == '\'' and text.lookAhead(2) == '\'') {
      Token* t = new Token();
      t->type = TOKEN_CHARACTER;
      text.subString(t->text, 3);
      return t;
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptSpecialCharacter() {
    const char* SPECIAL_CHARACTERS = ApplicationSpecificScanner::getSpecialCharacters();
    for (int i=0; i < ApplicationSpecificScanner::NUMBER_OF_SPECIAL_CHARACTERS; i++) {
      if (text.lookAhead(0) == SPECIAL_CHARACTERS[i]) {
        Token* t = new Token();
        t->type = TOKEN_SPECIAL_CHARACTER;
        text.subString(t->text, 1);
        return t;
      }
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::isKeyword(Text& t, Keyword& keyword) {
    static const typename ApplicationSpecificScanner::KeywordInfo* KEYWORD_INFO =
      ApplicationSpecificScanner::getKeywordInfo();
    static typename std::unordered_map<std::string, Keyword> hashMap;
    for (int i=0; i < ApplicationSpecificScanner::NUMBER_OF_KEYWORDS; i++) {
      std::string key = KEYWORD_INFO[i].text;
      hashMap[key] = KEYWORD_INFO[i].keyword;
    }
    std::string key = t.toString();
    if (!ApplicationSpecificScanner::CASE_SENSITIVE) {
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    }
    typename std::unordered_map<std::string, Keyword>::const_iterator got = hashMap.find(key);
    if ( got != hashMap.end() ) {
      keyword = got->second;
      return true;
    }
    return false;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptIdentifier() {
    static const int LUT_SIZE = sizeof(char) * 256;
    static const struct BasicIdentifierLookup
    {
      std::array<char, LUT_SIZE> VALID_FIRST_CHAR;
      std::array<char, LUT_SIZE> VALID_CHAR;

      void assignRange(std::array<char, LUT_SIZE>& lut, char start, char stop, char value) {
        for (int i=start; i <= stop; i++) {
          lut[i] = value;
        }
      }
      
      void assignRange(char start, char stop, char value) {
        for (int i=start; i <= stop; i++) {
          VALID_FIRST_CHAR[i] = VALID_CHAR[i] = value;
        }
      }

      BasicIdentifierLookup() {
        assignRange(0, LUT_SIZE-1, false);
        assignRange('a', 'z', true);
        assignRange('A', 'Z', true);
        assignRange(VALID_CHAR, '0', '9', true);
        assignRange(VALID_CHAR, '_', '_', true);
      }
    } BASIC_IDENTIFIER;
    int i = 0;
    char a = text.lookAhead(i);
    if (BASIC_IDENTIFIER.VALID_FIRST_CHAR[a]) {
      do {
        a = text.lookAhead(++i);
      } while (BASIC_IDENTIFIER.VALID_CHAR[a]);
    } else if (a == '\\') {
      bool extended_identifier_end_found = false;
      i++;
      do {
        if (text.lookAhead(i++) == '\\') {
          if (text.lookAhead(i) == '\\') {
            i++;
          } else {
            extended_identifier_end_found = true;
          }
        }
      } while (!extended_identifier_end_found);
    } else {
      return NULL;
    }
    Token* t = new Token();
    text.subString(t->text, i);
    Keyword k;
    if (isKeyword(t->text, k)) {
      t->type = TOKEN_KEYWORD;
      t->keyword = k;
      if (verbose) {
        std::cout << "Found keyword = " << t->text.toString() << std::endl;
      }
    } else {    
      t->type = TOKEN_IDENTIFIER;
      if (verbose) {
        std::cout << "Found basic identifier = " << t->text.toString() << std::endl;
      }
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptNumber() {
    static const int LUT_SIZE = sizeof(char) * 256;
    static const struct NumberLookup
    {
      std::array<char, LUT_SIZE> VALID_FIRST_CHAR;
      std::array<char, LUT_SIZE> VALID_CHAR;

      void assignRange(std::array<char, LUT_SIZE>& lut, char start, char stop, char value) {
        for (int i=start; i <= stop; i++) {
          lut[i] = value;
        }
      }
      
      void assignRange(char start, char stop, char value) {
        for (int i=start; i <= stop; i++) {
          VALID_FIRST_CHAR[i] = VALID_CHAR[i] = value;
        }
      }

      NumberLookup() {
        assignRange(0, LUT_SIZE-1, false);
        assignRange('0', '9', true);
        assignRange(VALID_CHAR, '.', '.', true);
        assignRange(VALID_CHAR, 'e', 'e', true);
        assignRange(VALID_CHAR, 'E', 'E', true);
        assignRange(VALID_CHAR, '+', '+', true);
      }
    } BASIC_IDENTIFIER;
    char a = text.lookAhead(0);
    if (!BASIC_IDENTIFIER.VALID_FIRST_CHAR[a]) {
      return NULL;
    }
    int i = 0;
    bool punctuationFound = false;
    do {
      a = text.lookAhead(++i);
      if (a == '.') {
        punctuationFound = true;
      }
    } while (BASIC_IDENTIFIER.VALID_CHAR[a]);
    Token* t = new Token();
    if (punctuationFound) {
      t->type = TOKEN_REAL;
    } else {
      t->type = TOKEN_INTEGER;
    }
    text.subString(t->text, i);
    if (verbose) {
      std::cout << "Found number = " << t->text.toString() << std::endl;
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::tokenize() {
    debug.functionStart("tokenize");
    tokenInfo.tokens = new Token *[text.getSize()];
    tokenInfo.position = 0;
    tokenInfo.size = 0;
    bool result = false;
    try {
      bool match;
      do {
	skippedWhiteSpace |= applicationSpecificScanner.skipWhiteSpaceAndComments(this);
        match =
          add(acceptString()) ||
          add(acceptCharacter()) ||
          add(acceptSpecialCharacter()) ||
          add(acceptIdentifier()) ||
          add(acceptNumber());
      } while (match);
      error("Unknown token");
    } catch (TextEof e) {
      result = true;
    }
    debug.functionEnd("tokenize = " + std::string(result ? "true" : "false"));
    return result;
  }
  
  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::setText(std::string& filename, char* s) {
    text.set(filename, s, ApplicationSpecificScanner::CASE_SENSITIVE);
    tokenize();
    if (verbose) {
      std::cout << "TOKENLIST:" << std::endl;
      int line = -1;
      for (int i=0; i<tokenInfo.size; i++) {
        int nextLine = tokenInfo.tokens[i]->text.getLine();
        if (nextLine != line) {
          line = nextLine;
          std::cout << std::endl << "line " << line << " : ";
        }
        std::cout << " [" << toString(tokenInfo.tokens[i]) << "]";
      }
      std::cout << std::endl << "TOKENLIST END." << std::endl;
    }
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::loadFile(std::string& filename) {
    if (verbose) {
      std::cout << "Reading file " << filename << std::endl;
    }
    this->filename = filename;
    FILE *f = fopen(filename.c_str(), "rb");
    if (f != NULL) {
      fseek(f, 0, SEEK_END);
      long fsize = ftell(f);
      fseek(f, 0, SEEK_SET);  //same as rewind(f);

      char *string = (char *)malloc(fsize + 1);
      fread(string, fsize, 1, f);
      fclose(f);

      string[fsize] = 0;
      setText(filename, string);
    } else {
      std::cerr << "File " << filename << " not found" << std::endl;
      throw FileNotFound();
    }
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::incrementPosition(int size) {
    text.incrementPosition(size);
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::getText(Text& t) {
    t = text;
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::setText(const Text& t) {
    text = t;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::getPosition() {
    return text.getPosition();
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::isWhiteSpace() {
    char a = text.lookAhead(0);
    if (a == ' ' || a == '\t' || a == '\n' || a == '\r') {
      return 1;
    }
    return 0;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::skipWhiteSpace() {
    debug.debug("skipWhiteSpace");
    int i = 0;
    while (isWhiteSpace()) {
      text.incrementPosition();
      i++;
    }
    return i;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::skipUntilEndOfLine() {
    int i = 0;
    while (text.lookAhead(0) != '\n') {
      text.incrementPosition();
      i++;
    }
    return i;
  }


  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::getNumberOfErrors() {
    return number_of_errors;
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::error(const std::string &s)
  {
    number_of_errors++;
    exceptions.printError(s, &text);
    throw SyntaxError();
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::warning(const std::string &s) {
    exceptions.printWarning(s, &text);
  }
  
  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::critical(const std::string &s) {
    exceptions.printError(s, &text);
    throw CriticalError();
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::getTokenPosition() {
    return tokenInfo.position;
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::setTokenPosition(int p) {
    tokenInfo.position = p;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::tokenLookAhead(int number) {
    debug.functionStart("tokenLookAhead, number = " + std::to_string(number));
    int i = tokenInfo.position + number;
    if (i >= tokenInfo.size) {
      debug.debug("Throwing TokenEof");
      throw TokenEof();
    }
    debug.functionEnd("tokenLookAhead = " + toString(tokenInfo.tokens[i]));
    return tokenInfo.tokens[i];
  }
  
  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::nextToken(int number) {
    tokenInfo.position += number;
  }

  template <class ApplicationSpecificScanner>
  std::string& TokenScanner<ApplicationSpecificScanner>::getFilename() {
    return filename;
  }


  template <class ApplicationSpecificScanner>
  std::string TokenScanner<ApplicationSpecificScanner>::toString(Keyword k) {
    const typename ApplicationSpecificScanner::KeywordInfo* KEYWORD_INFO =
      ApplicationSpecificScanner::getKeywordInfo();
    for (int i=0; i < ApplicationSpecificScanner::NUMBER_OF_KEYWORDS; i++) {
      if (k == KEYWORD_INFO[i].keyword) {
        return KEYWORD_INFO[i].text;
      }
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  std::string TokenScanner<ApplicationSpecificScanner>::toString(TokenType k) {
    switch (k) {
    case TOKEN_KEYWORD:
      return "keyword";
    case TOKEN_INTEGER:
      return "number";
    case TOKEN_REAL:
      return "real";
    case TOKEN_IDENTIFIER:
      return "identifier";
    case TOKEN_SPECIAL_CHARACTER:
      return "special character";
    case TOKEN_CHARACTER:
      return "character";
    case TOKEN_STRING:
      return "string";
    };
    return "unknown keyword";
  }

  template <class ApplicationSpecificScanner>
  std::string TokenScanner<ApplicationSpecificScanner>::currentTokenToString() {
    debug.functionStart("currentTokenToString");
    std::string s = toString(tokenLookAhead(0)); 
    debug.functionEnd("currentTokenToString = " + s);
    return s;
  }

  template <class ApplicationSpecificScanner>
  std::string TokenScanner<ApplicationSpecificScanner>::toString(Token* t) {
    std::string s = toString(t->type);
    switch (t->type) {
    case TOKEN_KEYWORD:
      s += ": " + toString(t->keyword);
      break;
    case TOKEN_INTEGER:
    case TOKEN_REAL:
      s += ": " + t->text.toString();
      break;
    case TOKEN_IDENTIFIER:
      s += ": " + t->text.toString();
      break;
    case TOKEN_SPECIAL_CHARACTER:
      s += ": " + t->text.toString();
      break;
    };
    return s;
  }

  template <class ApplicationSpecificScanner>
  std::string TokenScanner<ApplicationSpecificScanner>::toString(Text* t) {
    std::string s = (t ? t->toString() : "NULL");
    return s;
  }

}

#endif

    
