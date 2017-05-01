
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
#include "../exceptions/exceptions.hpp"

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

    Exceptions exceptions;
    
    template <typename Func>
    void print(Func print) {
      Token* t = tokenLookAhead(0);
      std::string message = "Found " + toString(t);
      if (verbose) {
        message += "[" + std::to_string(getTokenPosition()) + "]";
      }
      print(&t->text, message);
    }

    void printCurrentLine();
    
    void debug(const std::string &message);

    int number_of_errors = 0;
    
    using Keyword = typename ApplicationSpecificScanner::Keyword;

    using TokenScanner<ApplicationSpecificScanner>::verbose;
    
  public:

    int accept(const char *text);
    int optional(const char* text);
    bool optional(const char text);
    bool lookAhead(const char* text, int n);
    int expect(const char* text);
    int expect(Text& text);
    bool match(const char text, int lookAhead = 0);
    int match(const char* text, int lookAhead = 0);

    Text* optional(Keyword keyword);
    Text* optional(TokenType type);
    Text* accept(Keyword keyword);
    Text* accept(TokenType type);
    Text* expect(Keyword keyword);
    Text* expect(TokenType type);

    Text* getCurrentTextPosition();
    
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
    using TokenScanner<ApplicationSpecificScanner>::toString;
    using TokenScanner<ApplicationSpecificScanner>::currentTokenToString;
    
    template<typename T>
    T* optional() {
      T* p = new T();
      int position = getTokenPosition();
      if (verbose) {
        std::cout << "[START]Optional[" << position << "]: " << typeid(p).name() << std::endl;
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
        std::cout << "[END]Optional[" << getTokenPosition() << "]: " << typeid(p).name() << " : ";
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
      List<T>* list = new List<T>();
      T* a;
      while(a = optional<T>()) {
        list->add(a);
      }
      return list;
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
  void Scanner<ApplicationSpecificScanner>::error(const std::string &s)
  {
    number_of_errors++;
    print([&](ast::Text* text, std::string& message) {
        exceptions.printError(message + s, text);
      });
    throw SyntaxError();
  }

  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::warning(const std::string &s) {
    print([&](ast::Text* text, std::string& message) {
        exceptions.printWarning(message + s, text);
      });
  }
  
  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::critical(const std::string &s) {
    print([&](ast::Text* text, std::string& message) {
        exceptions.printError(message + s, text);
      });
    throw CriticalError();
  }

  template <class ApplicationSpecificScanner>
  bool Scanner<ApplicationSpecificScanner>::match(const char a, int lookAhead) {
    DEBUG("match '" + std::to_string(a) + "'");
    try {
      Token* t = tokenLookAhead(lookAhead);
      DEBUG("Token = " + toString(t));
      if ((t->type != TOKEN_SPECIAL_CHARACTER) ||
          (t->text.lookAhead(0) != a)) {
        return false;
      }
    } catch (...) {return false;}
    return true;
  }

  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::match(const char* a, int lookAhead) {
    DEBUG("match '" + std::string(a) + "'");
    int len = strlen(a);
    for (int i=0; i<len; i++) {
      if (!match(a[i], i + lookAhead)) {
        return 0;
      }
    }
    return len;
  }

  template <class ApplicationSpecificScanner>
  bool Scanner<ApplicationSpecificScanner>::lookAhead(const char* t, int n) {
    int len = match(t, n);
    return len > 0;
  }

  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::printCurrentLine() {
    std::cout << tokenLookAhead(0)->text.getCurrentLine() << std::endl;
    std::cout << tokenLookAhead(0)->text.getCurrentLinePositionMarker() << std::endl;
  }
  
  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::getCurrentTextPosition() {
    return &tokenLookAhead(0)->text;
  }
  
  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::optional(const char* t) {
    int len = match(t);
    DEBUG("optional '" + std::string(t) + "'. len = " + std::to_string(len));
    if (verbose) {
      printCurrentLine();
    }
    if (len > 0) {
      nextToken(len);
    }
    return len;
  }
  
  template <class ApplicationSpecificScanner>
  bool Scanner<ApplicationSpecificScanner>::optional(const char t) {
    bool m = match(t);
    DEBUG("optional '" + std::to_string(t) + "'");
    if (verbose) {
      printCurrentLine();
    }
    if (m) {
      nextToken();
    }
    return m;
  }
  
  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::accept(const char* t) {
    DEBUG("accept '" + std::string(t) + "'");
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
  int Scanner<ApplicationSpecificScanner>::expect(const char* t) {
    DEBUG("expect '" + std::string(t) + "'");
    int len = optional(t);
    if (len == 0) {
      error("Expected '" + std::string(t) + "'");
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

    
