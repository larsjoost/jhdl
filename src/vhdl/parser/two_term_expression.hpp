
//#ifndef VHDL_PARSER_EXPRESSION_H_
//#define VHDL_PARSER_EXPRESSION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/two_term_expression.hpp"

namespace vhdl {
  namespace parser {

    class TwoTermExpression :
      public ::ast::TwoTermExpression  {

    public:

      TwoTermExpression* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

//#endif
