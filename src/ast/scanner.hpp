
#ifndef AST_SCANNER_HPP_
#define AST_SCANNER_HPP_

#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <exception>
#include <iostream>
#include <typeinfo>

#include "text.hpp"
#include "list.hpp"
#include "identifier.hpp"
#include "token_scanner.hpp"

namespace ast {

  class UnexpectedToken {
  public:
    Text text;
    UnexpectedToken(Text t) {
      text = t;
    }
  };
  class NoWhiteSpace {
  public:
    char letter;
    NoWhiteSpace(char c) {
      letter = c;
    }
  };
  
  template <class ApplicationSpecificScanner>
  class Scanner : public TokenScanner<ApplicationSpecificScanner>{

    void print(const std::string &severity, const std::string &message);

    void debug(const std::string &message);

    int verbose;

    int number_of_errors = 0;
    
    using Keyword = typename ApplicationSpecificScanner::Keyword;
 
  public:

    Scanner(int verbose = 0);
        
    int accept(const char *text);
    int optional(const char* text);
    int expect(const char* text);
    int expect(Text& text);
    int match(const char* text);

    Text* optional(Keyword keyword);
    Text* optional(TokenType type);
    Text* accept(Keyword keyword);
    Text* accept(TokenType type);
    Text* expect(Keyword keyword);
    Text* expect(TokenType type);

    bool isWhiteSpace();
    void skipWhiteSpace();
    
    void warning(const std::string &s);
    void error(const std::string &s);
    void critical(const std::string &s);

    using typename TokenScanner<ApplicationSpecificScanner>::Token;
    
    using TokenScanner<ApplicationSpecificScanner>::getTokenPosition;
    using TokenScanner<ApplicationSpecificScanner>::setTokenPosition;
    using TokenScanner<ApplicationSpecificScanner>::tokenLookAhead;
    using TokenScanner<ApplicationSpecificScanner>::nextToken;
    using TokenScanner<ApplicationSpecificScanner>::loadFile;
    using TokenScanner<ApplicationSpecificScanner>::getFilename;
    using TokenScanner<ApplicationSpecificScanner>::printTextLinePosition;
    using TokenScanner<ApplicationSpecificScanner>::toString;
    using TokenScanner<ApplicationSpecificScanner>::currentTokenToString;
    
    template<typename T>
    T* optional() {
      T* p = new T();
      int position = getTokenPosition();
      if (verbose) {
        std::cout << "Optional[" << position << "]: " << typeid(p).name() << std::endl;
      }
      try {
        p->parse(this);
      } catch (TokenNotAccepted e) {
        setTokenPosition(position);
        p = NULL;
      }
      if (getTokenPosition() == position) {
        p = NULL;
      }
      if (verbose) {
        std::cout << "Optional[" << getTokenPosition() << "]: " << typeid(p).name() << " : ";
        std::cout << (p ? "[FOUND]" : "[MISSED]");
        std::cout << " Token = " + currentTokenToString() << std::endl;
      }
      return p; 
    }

    template<typename T>
    T* expect() {
      T* p = optional<T>();
      if (verbose) {
        std::cout << "Expect: " << typeid(p).name() << std::endl;
      }
      if (!p) {
        error("Expected something else");
      }
      return p; 
    }
  
    template<typename T>
    T* accept() {
      T* p = optional<T>();
      if (verbose) {
        std::cout << "Accept: " << typeid(p).name() << std::endl;
      }
      if (!p) {
        throw TokenNotAccepted();
      }
      return p; 
    }
    
    template<typename T>
    List<T*>* zeroOrMore() {
      List<T> list = new List<T>();
      T* a;
      while(a = optional<T>()) {
        list.add(a);
      }
      return &list;
    }
    
    template<typename T>
    List<T*>* oneOrMore() {
      T* a = expect<T>();
      List<T*>* list = zeroOrMore<T>();
      list->add(a);
      return list;
    }
    
  };  

  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::print(const std::string &severity, const std::string &text) {
    Token* t = tokenLookAhead(0);
    std::cerr << severity << " in file " << getFilename() << " at "
	      << std::to_string(t->text.getLine()) << ", "
	      << std::to_string(t->text.getColumn()) << ": "
	      << text
              << ". Found token";
    if (verbose) {
      std::cerr << "[" << getTokenPosition() << "]";
    }
    std::cerr << ": " + toString(t) + "."
              << std::endl;
    
    printTextLinePosition(stderr, t->text);
  }

  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::error(const std::string &s)
  {
    number_of_errors++;
    print("error", s);
    throw SyntaxError();
  }

  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::warning(const std::string &s) {
    print("warning", s);
  }
  
  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::critical(const std::string &s) {
    print("critical", s);
    throw CriticalError();
  }

  template <class ApplicationSpecificScanner>
  Scanner<ApplicationSpecificScanner>::Scanner(int v) :
    Scanner<ApplicationSpecificScanner>::TokenScanner(v){
    verbose = v;
  }

  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::match(const char* a) {
    DEBUG("match '" + std::string(a) + "'");
    int len = strlen(a);
    try {
      for (int i=0; i<len; i++) {
        Token* t = tokenLookAhead(i);
        DEBUG("Token = " + toString(t));
        if ((t->type != TOKEN_SPECIAL_CHARACTER) ||
            (t->text.lookAhead(0) != a[i])) {
          return 0;
        }
      }
    } catch (...) {return 0;}
    return len;
  }

  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::optional(const char* t) {
    int len = match(t);
    DEBUG("optional '" + std::string(t) + "'. len = " + std::to_string(len));
    if (len > 0) {
      nextToken(len);
    }
    return len;
  }
  
  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::accept(const char* t) {
    DEBUG("accept" + std::string(t));
    int len = optional(t);
    if (len == 0) {
      throw TokenNotAccepted();
    }
    return len;
  }


  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::optional(Keyword keyword) {
    DEBUG("Optional keyword = " + toString(keyword));
    Token* t = tokenLookAhead(0);
    if (t->type == TOKEN_KEYWORD && t->keyword == keyword) {
      nextToken();
      return &t->text;
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::accept(Keyword keyword) {
    DEBUG("Accept keyword = " + toString(keyword));
    Text* t;
    if (!(t = optional(keyword))) {
      throw TokenNotAccepted();
    }
    return t;
  }
  
  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::expect(Keyword keyword) {
    DEBUG("Expect keyword = " + toString(keyword));
    Text* t;
    if (!(t = optional(keyword))) {
      error("Expected " + toString(keyword));
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  bool Scanner<ApplicationSpecificScanner>::isWhiteSpace() {
    Token* t = tokenLookAhead(0);
    if (t->type == TOKEN_WHITESPACE) {
      return true;
    }
    return false;
  }
    
  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::skipWhiteSpace() {
    while (isWhiteSpace()) {
      nextToken();
    }
  }

  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::expect(const char* t) {
    DEBUG("expect" + std::string(t));
    int len = optional(t);
    if (len == 0) {
      error("Expected '" + std::string(t));
    }
    return len;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::optional(TokenType type) {
    DEBUG("Optional token = " + toString(type));
    Token* t = tokenLookAhead(0);
    if (t->type == type) {
      nextToken();
      return &t->text;
    }
    return NULL;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::accept(TokenType type) {
    DEBUG("Accept token = " + toString(type));
    Text* t;
    if (!(t = optional(type))) {
      throw TokenNotAccepted();
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::expect(TokenType type) {
    DEBUG("Expect token = " + toString(type));
    Text* t;
    if (!(t = optional(type))) {
      error("Expected " + toString(type));
    }
    return t;
  }

}

#endif

    
