#ifndef VHDL_PARSER_BASICIDENTIFIERLIST_H_
#define VHDL_PARSER_BASICIDENTIFIERLIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/basic_identifier_list.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {

    class CommaSeparation {
    public:
      char* getSeparator() { 
        return (char *)",";
      }
    };
    
    class PunctuationSeparation {
    public:
      char* getSeparator() { 
        return (char *)".";
      }
    };

    template <class T>
    class BasicIdentifierList :
      public ::ast::BasicIdentifierList {
    
    public:
      
      BasicIdentifierList<T>* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

    template <class T>
    BasicIdentifierList<T>* BasicIdentifierList<T>::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      textList.add(scanner->accept<BasicIdentifier>());
      T t;
      while (scanner->optional(t.getSeparator())) {
        textList.add(scanner->expect<BasicIdentifier>());
      };
      this;
    }

  }
}

#endif
