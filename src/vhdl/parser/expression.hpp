
#ifndef VHDL_PARSER_EXPRESSION_H_
#define VHDL_PARSER_EXPRESSION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/expression.hpp"

namespace vhdl {
  namespace parser {

    class Expression :
      public ::ast::Expression  {

    public:

      Expression* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
