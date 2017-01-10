
#ifndef VHDL_SCANNER_SCANNER_H_
#define VHDL_SCANNER_SCANNER_H_

#include <array>

#include "../../ast/scanner.hpp"
#include "../../ast/identifier.hpp"
#include "../../ast/list.hpp"
#include "defines.hpp"

namespace vhdl {
  namespace scanner {
  
    class BasicIdentifier : public ::ast::Identifier {
    public:
      using ::ast::Identifier::equals;
      using ::ast::Identifier::print;
      using ::ast::Identifier::debug;
    };

    class Scanner : public ::ast::Scanner {
  
    public:
    
      int verbose;

      Scanner(int verbose = 0);
    
      void accept(VhdlKeyword keyword);
      int accept(BasicIdentifier& i);
      void expect(VhdlKeyword keyword);
      void expect(BasicIdentifier& i);
      int optional(VhdlKeyword keyword);
      
      void skipWhiteSpaceAndComments();
      
      using ::ast::Scanner::getPosition;
      using ::ast::Scanner::accept;
      using ::ast::Scanner::expect;
      using ::ast::Scanner::optional;
      using ::ast::Scanner::getText;
      using ::ast::Scanner::lookAhead;
      using ::ast::Scanner::eat;
      using ::ast::Scanner::incrementPosition;
      using ::ast::Scanner::getNumberOfErrors;
      using ::ast::Scanner::skipOneOrMoreWhiteSpaces;

      template<typename T> T* expect();
      template<typename T> T* accept();
      template<typename T> T* optional();
      template<typename T> ::ast::List<T*>* zeroOrMore();
      template<typename T> ::ast::List<T*>* oneOrMore();

    };

    template<typename T>
    T* Scanner::expect() {
      T* p = new T();
      p->parse(this);
      return p; 
    }

    template<typename T>
    T* Scanner::accept() {
      try {
        return expect<T>();
      } catch (ast::TokenNotAccepted e) {
      }
      return NULL;
    }
    
    template<typename T>
    T* Scanner::optional() {
      try {
        return expect<T>();
      } catch (ast::UnexpectedToken e) {
      }
      return NULL;
    }

    template<typename T>
    ::ast::List<T*>* Scanner::zeroOrMore() {
      ::ast::List<T> list = new ::ast::List<T>();
      T* a;
      while(a = optional<T>()) {
        list.add(a);
      }
      return &list;
    }
    
    template<typename T>
    ::ast::List<T*>* Scanner::oneOrMore() {
      T* a = expect<T>();
      ::ast::List<T*>* list = zeroOrMore<T>();
      list->add(a);
      return list;
    }
    
  }
}

#endif

