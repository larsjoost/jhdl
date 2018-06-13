#ifndef VERILOG_PARSER_LIST_H_
#define VERILOG_PARSER_LIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/list.hpp"

namespace verilog {
  namespace parser {

    template <class Type, class AstType, char separator>
    class List :
      public ::ast::List<AstType> {

      using ::ast::List<AstType>::add;
      
    public:

      void parse(::ast::Scanner<scanner::Scanner>* scanner) {
        add(scanner->accept<Type>());
        while (scanner->optional(separator)) {
          add(scanner->expect<Type, List>());
        };
      }
    };

  }
}

#endif
