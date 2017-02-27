
#ifndef VHDL_PARSER_EXPRESSIONTERM_H_
#define VHDL_PARSER_EXPRESSIONTERM_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/expression_term.hpp"

namespace vhdl {
  namespace parser {

    class ExpressionTerm :
      public ::ast::ExpressionTerm  {

    public:

      ExpressionTerm* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
