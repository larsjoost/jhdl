
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

#include "../debug/debug.hpp"
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

    Debug<false> m_debug;
    Exceptions exceptions;
    
    template <typename Func>
    void print(Func print) {
      Token* t = tokenLookAhead(0);
      std::string message = "Found " + toString(t) + ". ";
      if (verbose) {
        message += "[" + std::to_string(getTokenPosition()) + "]";
      }
      print(&t->text, message);
    }

    void printCurrentLine();
    
    int number_of_errors = 0;
    
    using Keyword = typename ApplicationSpecificScanner::Keyword;

    using TokenScanner<ApplicationSpecificScanner>::verbose;
    
  public:

    Scanner() : m_debug("ast::Scanner") {}
    
    int accept(const char *text);
    int optional(const char* text);
    bool optional(const char text);
    bool lookAhead(const char* text, int n);
    int expect(const char* text);
    int expect(Text& text);
    bool match(const char text, int lookAhead = 0);
    int match(const char* text, int lookAhead = 0);

    Text* LookAhead(int number, TokenType keyword);
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
    inline T* optional() {
      T* p = new T();
      int position = getTokenPosition();
      m_debug.functionStart("Optional[" + std::to_string(position) + "]: " + typeid(p).name(), true);
      try {
        p->parse(this);
        m_debug.debug("Parse successfull");
      } catch (TokenNotAccepted& e) {
        setTokenPosition(position);
        p = NULL;
      } catch (SyntaxError& e) {
        std::cout << "Syntax error backtrace at " << typeid(T).name() << std::endl;
        throw e;
      }
      if (getTokenPosition() == position) {
        p = NULL;
      }
      m_debug.functionEnd("Optional[" + std::to_string(getTokenPosition()) + "]: " + typeid(p).name() + " : " +
                        (p ? "[FOUND]" : "[MISSED]"), true);
      return p; 
    }

    template<typename T, typename Parent>
    inline T* expect() {
      T* p = optional<T>();
      if (verbose) {
        std::cout << "Expect: " << typeid(p).name() << std::endl;
      }
      if (!p) {
        error(std::string(typeid(Parent).name()) + std::string(" expected ") + std::string(typeid(T).name()));
      }
      return p; 
    }
  
    template<typename T>
    inline T* accept() {
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
    inline List<T*>* zeroOrMore() {
      List<T>* list = new List<T>();
      T* a;
      while(a = optional<T>()) {
        list->add(a);
      }
      return list;
    }
    
    template<typename T>
    inline List<T*>* oneOrMore() {
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
    m_debug.functionStart("match a = '" + std::string(1, a) + "'");
    bool result = true;
    try {
      Token* t = tokenLookAhead(lookAhead);
      m_debug.debug("Token = " + toString(t));
      if ((t->type != TOKEN_SPECIAL_CHARACTER) ||
          (t->text.lookAhead(0) != a)) {
        result = false;
      }
    } catch (...) {result = false;}
    m_debug.functionEnd("match = " + std::string(result ? "true" : "false"));
    return result;
  }

  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::match(const char* a, int lookAhead) {
    m_debug.functionStart("match a = \"" + std::string(a) + "\"");
    int len = strlen(a);
    for (int i=0; i<len; i++) {
      if (!match(a[i], i + lookAhead)) {
        len = 0;
        break;
      }
    }
    m_debug.functionEnd("match = " + std::to_string(len));
    return len;
  }

  template <class ApplicationSpecificScanner>
  bool Scanner<ApplicationSpecificScanner>::lookAhead(const char* t, int n) {
    int len = match(t, n);
    return len > 0;
  }

  template <class ApplicationSpecificScanner>
  void Scanner<ApplicationSpecificScanner>::printCurrentLine() {
    m_debug.functionStart("printCurrentLine");
    std::cout << tokenLookAhead(0)->text.getCurrentLine() << std::endl;
    std::cout << tokenLookAhead(0)->text.getCurrentLinePositionMarker() << std::endl;
    m_debug.functionEnd("printCurrentLine");
  }
  
  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::getCurrentTextPosition() {
    m_debug.functionStart("getCurrentTextPosition");
    Text* t = &tokenLookAhead(0)->text;
    m_debug.functionEnd("getCurrentTextPosition = " + toString(t));
    return t;
  }
  
  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::optional(const char* t) {
    int len = match(t);
    m_debug.debug("optional '" + std::string(t) + "'. len = " + std::to_string(len));
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
    m_debug.debug("optional '" + std::to_string(t) + "'");
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
    m_debug.debug("accept '" + std::string(t) + "'");
    int len = optional(t);
    if (len == 0) {
      throw TokenNotAccepted();
    }
    return len;
  }


  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::optional(Keyword keyword) {
    m_debug.functionStart("Optional, keyword = " + toString(keyword));
    Token* token = tokenLookAhead(0);
    Text* result = NULL;
    if (token->type == TOKEN_KEYWORD && token->keyword == keyword) {
      nextToken();
      result = &token->text;
    }
    m_debug.functionEnd("Optional = " + toString(result));
    return result;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::LookAhead(int number, TokenType type) {
    m_debug.functionStart("LookAhead, type = " + toString(type));
    Token* token = tokenLookAhead(number);
    Text* result = NULL;
    if (token->type == type) {
      result = &token->text;
    }
    m_debug.functionEnd("LookAhead = " + toString(result));
    return result;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::accept(Keyword keyword) {
    m_debug.debug("Accept keyword = " + toString(keyword));
    Text* t;
    if (!(t = optional(keyword))) {
      throw TokenNotAccepted();
    }
    return t;
  }
  
  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::expect(Keyword keyword) {
    m_debug.debug("Expect keyword = " + toString(keyword));
    Text* t;
    if (!(t = optional(keyword))) {
      error("Expected " + toString(keyword));
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  int Scanner<ApplicationSpecificScanner>::expect(const char* t) {
    m_debug.functionStart("expect '" + std::string(t) + "'");
    int len = optional(t);
    if (len == 0) {
      error("Expected '" + std::string(t) + "'");
    }
    m_debug.functionEnd("expect = " + std::to_string(len));
    return len;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::optional(TokenType type) {
    m_debug.functionStart("Optional, token = " + toString(type));
    Token* token = tokenLookAhead(0);
    Text* result = NULL;
    if (token->type == type) {
      nextToken();
      result = &token->text;
    }
    m_debug.functionEnd("Optional = " + toString(result));
    return result;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::accept(TokenType type) {
    m_debug.debug("Accept token = " + toString(type));
    Text* t;
    if (!(t = optional(type))) {
      throw TokenNotAccepted();
    }
    return t;
  }

  template <class ApplicationSpecificScanner>
  Text* Scanner<ApplicationSpecificScanner>::expect(TokenType type) {
    m_debug.debug("Expect token = " + toString(type));
    Text* t;
    if (!(t = optional(type))) {
      error("Expected " + toString(type));
    }
    return t;
  }

}

#endif

    
