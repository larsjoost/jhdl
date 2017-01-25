
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

#include "text.hpp"
#include "list.hpp"
#include "identifier.hpp"

#define DEBUG_ENABLE

namespace ast {

  class SyntaxError : public std::exception {};
  class CriticalError : public std::exception {};
  class FileNotFound : public std::exception {};
  class TokenEof : public std::exception {};
  class TokenNotAccepted : public std::exception {};

  enum TokenType {
    TOKEN_KEYWORD,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_SPECIAL_CHARACTER,
    TOKEN_WHITESPACE
  };

  template <class ApplicationSpecificScanner>
  class TokenScanner {

  public:

    using Keyword = typename ApplicationSpecificScanner::Keyword;
    
    struct Token {
      TokenType type;
      Keyword keyword;
      Text text;
    };

  private:
    
    std::string filename;
    Text* text;

    struct TokenInfo {
      Token** tokens;
      int position;
      int size;
    };

    TokenInfo tokenInfo;
    
    ApplicationSpecificScanner applicationSpecificScanner;
    
    int number_of_errors = 0;
    
    void print(const std::string &severity, const std::string &message);

#ifdef DEBUG_ENABLE
#define DEBUG(x) do {						   \
      if (verbose) {						   \
	std::cerr << "ast::Tokenscanner.cpp: " << x << std::endl;	   \
      }} while(false)
#else
#define DEBUG(x) 
#endif
    void debug(const std::string &message);

    int verbose;

    Token* acceptWhiteSpace();
    Token* acceptSpecialCharacter();
    Token* acceptKeyword();
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

    TokenScanner(int verbose = 0);
        
    int isWhiteSpace();
    int skipWhiteSpace();
    int skipUntilEndOfLine();

    bool accept(const char* c);
    
    void setText(char* s);
    void loadFile(const char* filename);
    
    void getText(Text& t);
    void setText(const Text& t);
    
    void warning(const std::string &s);
    void error(const std::string &s);
    void critical(const std::string &s);

    int getNumberOfErrors();

    Token* tokenLookAhead(int number);
    void nextToken(int number = 1);
    int getTokenPosition();

    std::string& getFilename();
    
    void printTextLinePosition(FILE* output, Text& t);

    std::string toString(Token* t);
    std::string toString(Keyword k);
    std::string toString(TokenType k);
    
  };  


  template <class ApplicationSpecificScanner>
  TokenScanner<ApplicationSpecificScanner>::TokenScanner(int v) {
    verbose = v;
    text = new Text(v);
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
    DEBUG("match " + std::string(a));
    int len = strlen(a);
    for (int i=0; i<len; i++) {
      char c = text->lookAhead(i);
      if (!compare(c, a[i])) {
        return false;
      }
    }
    text->subString(t, len);
    return true;
  }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::optional(Text& t, const char* c) {
    DEBUG("optional " + std::string(c));
    bool result; 
    if (result = match(t, c)) {
      incrementPosition(t.remainingSize());
    }
    return result;
  }
   
  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::accept(const char* c) {
     DEBUG("accept" + std::string(c));
     Text t;
     if (!optional(t, c)) {
       throw TokenNotAccepted();
     }
     return true;
   }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::add(Token* token) {
    if (token) {
      DEBUG("Added token " + token->text.toString());
      tokenInfo.tokens[tokenInfo.size] = token;
      tokenInfo.size++;
      text->incrementPosition(token->text.remainingSize());
      return true;
    }
    return false;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptWhiteSpace() {
    if (applicationSpecificScanner.skipWhiteSpaceAndComments(this)) {
      Token* t = new Token();
      t->type = TOKEN_WHITESPACE;
      text->subString(t->text, 0);
      return t;
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptSpecialCharacter() {
    const char* SPECIAL_CHARACTERS = ApplicationSpecificScanner::getSpecialCharacters();
    for (int i=0; i < ApplicationSpecificScanner::NUMBER_OF_SPECIAL_CHARACTERS; i++) {
      if (text->lookAhead(0) == SPECIAL_CHARACTERS[i]) {
        Token* t = new Token();
        t->type = TOKEN_SPECIAL_CHARACTER;
        text->subString(t->text, 1);
        return t;
      }
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::acceptKeyword() {
    const typename ApplicationSpecificScanner::KeywordInfo* KEYWORD_INFO =
      ApplicationSpecificScanner::getKeywordInfo();
    for (int i=0; i < ApplicationSpecificScanner::NUMBER_OF_KEYWORDS; i++) {
      Text a;
      if (match(a, KEYWORD_INFO[i].text)) {
        Token* t = new Token();
        t->type = TOKEN_KEYWORD;
        t->keyword = KEYWORD_INFO[i].keyword;
        t->text = a;
        return t;
      }
    }
    return NULL;
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
    char a = text->lookAhead(0);
    if (!BASIC_IDENTIFIER.VALID_FIRST_CHAR[a]) {
      return NULL;
    }
    int i = 0;
    do {
      a = text->lookAhead(++i);
    } while (BASIC_IDENTIFIER.VALID_CHAR[a]);
    Token* t = new Token();
    t->type = TOKEN_IDENTIFIER;
    text->subString(t->text, i);
    if (verbose) {
      std::cout << "Found basic identifier = " << t->text.toString() << std::endl;
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
        assignRange('.', '.', true);
        assignRange(VALID_FIRST_CHAR, '-', '-', true);
      }
    } BASIC_IDENTIFIER;
    char a = text->lookAhead(0);
    if (!BASIC_IDENTIFIER.VALID_FIRST_CHAR[a]) {
      return NULL;
    }
    int i = 0;
    do {
      a = text->lookAhead(++i);
    } while (BASIC_IDENTIFIER.VALID_CHAR[a]);
    Token* t = new Token();
    t->type = TOKEN_NUMBER;
    text->subString(t->text, i);
    if (verbose) {
      std::cout << "Found number = " << t->text.toString() << std::endl;
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  bool TokenScanner<ApplicationSpecificScanner>::tokenize() {
    tokenInfo.tokens = new Token *[text->getSize()];
    tokenInfo.position = 0;
    tokenInfo.size = 0;
    try {
      bool match;
      do {
        match =
          add(acceptWhiteSpace()) || 
          add(acceptKeyword()) ||
          add(acceptSpecialCharacter()) ||
          add(acceptIdentifier()) ||
          add(acceptNumber());
      } while (match);
    } catch (TextEof e) {
      return true;
    }
    return false;
  }
  
  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::setText(char* s) {
    text->set(s);
    tokenize();
    if (verbose) {
      std::cout << "TOKENLIST:" << std::endl;
      for (int i=0; i<tokenInfo.size; i++) {
        std::cout << "TOKEN[" << i << "]: " + toString(tokenInfo.tokens[i]) << std::endl;
      }
      std::cout << "TOKENLIST END." << std::endl;
    }
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::loadFile(const char* filename) {
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
      setText(string);
    } else {
      throw FileNotFound();
    }
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::incrementPosition(int size) {
    text->incrementPosition(size);
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::getText(Text& t) {
    t = *text;
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::setText(const Text& t) {
    *text = t;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::getPosition() {
    return text->getPosition();
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::isWhiteSpace() {
    char a = text->lookAhead(0);
    if (a == ' ' || a == '\t' || a == '\n' || a == '\r') {
      return 1;
    }
    return 0;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::skipWhiteSpace() {
    DEBUG("skipWhiteSpace");
    int i = 0;
    while (isWhiteSpace()) {
      text->incrementPosition();
      i++;
    }
    return i;
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::skipUntilEndOfLine() {
    int i = 0;
    while (text->lookAhead(0) != '\n') {
      text->incrementPosition();
      i++;
    }
    return i;
  }


  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::getNumberOfErrors() {
    return number_of_errors;
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::printTextLinePosition(FILE* output, Text& t) {
    t.printLinePosition(output);
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::print(const std::string &severity, const std::string &text) {
    std::cerr << severity << " in file " << filename << " at "
              << std::to_string(this->text->getLine()) << ", "
              << std::to_string(this->text->getColumn()) << ": "
              << text << std::endl;
    printTextLinePosition(stderr, this->text);
  }
  
  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::error(const std::string &s)
  {
    number_of_errors++;
    print("error", s);
    throw SyntaxError();
  }

  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::warning(const std::string &s) {
    print("warning", s);
  }
  
  template <class ApplicationSpecificScanner>
  void TokenScanner<ApplicationSpecificScanner>::critical(const std::string &s) {
    print("critical", s);
    throw CriticalError();
  }

  template <class ApplicationSpecificScanner>
  int TokenScanner<ApplicationSpecificScanner>::getTokenPosition() {
    return tokenInfo.position;
  }

  template <class ApplicationSpecificScanner>
  typename TokenScanner<ApplicationSpecificScanner>::Token*
  TokenScanner<ApplicationSpecificScanner>::tokenLookAhead(int number) {
    int i = tokenInfo.position + number;
    if (i >= tokenInfo.size) {
      throw TokenEof();
    }
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
    case TOKEN_NUMBER:
      return "number";
    case TOKEN_IDENTIFIER:
      return "identifier";
    case TOKEN_SPECIAL_CHARACTER:
      return "special character";
    case TOKEN_WHITESPACE:
      return "white-space";
    };
    return "unknown keyword";
  }

  template <class ApplicationSpecificScanner>
  std::string TokenScanner<ApplicationSpecificScanner>::toString(Token* t) {
    std::string s = toString(t->type);
    switch (t->type) {
    case TOKEN_KEYWORD:
      s += ": " + toString(t->keyword);
      break;
    case TOKEN_NUMBER:
      s += ": " + t->text.toString();
      break;
    case TOKEN_IDENTIFIER:
      s += ": " + t->text.toString();
      break;
    case TOKEN_SPECIAL_CHARACTER:
      s += ": " + t->text.toString();
      break;
    case TOKEN_WHITESPACE:
      break;
    };
    return s;
  }

}

#endif

    
