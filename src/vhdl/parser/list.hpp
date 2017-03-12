#ifndef VHDL_PARSER_LIST_H_
#define VHDL_PARSER_LIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/list.hpp"

namespace vhdl {
  namespace parser {

    template <class Type, char separator>
    class List :
      public ::ast::List<Type> {

      using ::ast::List<Type>::add;
      
    public:

      ::ast::List<Type>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        add(scanner->accept<Type>());
        while (scanner->optional(separator)) {
          add(scanner->expect<Type>());
        };
        this;
      }
    };

  }
}

#endif
