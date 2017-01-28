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
      
      void parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

    template <class T>
    void BasicIdentifierList<T>::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      textList.add(&i->text);
      scanner->skipWhiteSpace();
      T t;
      while (scanner->optional(t.getSeparator())) {
        scanner->skipWhiteSpace();
        i = scanner->expect<BasicIdentifier>();
        textList.add(&i->text);
        scanner->skipWhiteSpace();
      };
    }

  }
}

#endif
