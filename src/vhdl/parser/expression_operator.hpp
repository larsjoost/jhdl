
#ifndef VHDL_PARSER_EXPRESSION_OPERATOR_H_
#define VHDL_PARSER_EXPRESSION_OPERATOR_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/expression_operator.hpp"

namespace vhdl {
  namespace parser {

    class ExpressionOperator :
      public ::ast::ExpressionOperator  {

    public:

      ExpressionOperator* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
